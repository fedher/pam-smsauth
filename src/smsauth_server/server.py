#!/usr/bin/python
#
# (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
#
import sys
import socket
import posix
import simplejson
import client
import proto
import client_factory
import config
import logging


SRV_CONF_FILE = "/etc/smsauth/server.conf"


class SMSAuthServer:
	
	def __init__(self, sockPath, srv_conf_file):
		self.__config = srv_conf_file
		
		try:
			self.__logger = logging.getLogger("smsauth_server")
			self.__logger.setLevel(logging.DEBUG)

			# Create console handler and set level to debug.
			fh = logging.FileHandler("/var/log/smsauth/smsauth_server.log")
			fh.setLevel(logging.DEBUG)

			# Create formatter.
			formatter = logging.Formatter("%(asctime)s - %(name)s - %(levelname)s -"
				   + "%(message)s")

			# Add formatter to ch.
			fh.setFormatter(formatter)

			# Add ch to logger.
			self.__logger.addHandler(fh)

		except Exception, e:
			print str(e)

		# Sets the gateway priority.
		try:
			conf = config.ConfFileParser(srv_conf_file, config.CONF_PRIORITY_SECTION)
			self.__gw_priority = conf.get_gw_priority()

		except Exception, e:
			self.__logger.error(e)
			sys.exit(1)

		# Creates and sets the socket.
		try:
			self.__sockd = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
			self.__sockd.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
			# bind() can fail if the pathname exists. So execute unlink() to delete.
			posix.unlink(sockPath);

		except OSError:
			# posix.unlink(): unix sock doesn't exist.
			pass

		except socket.error, e:
			self.__logger.error(str(e))
			sys.exit(1)

		self.__sockd.bind(str(sockPath))
		self.__sockd.listen(5)


	def __exec_client(self, id):
		factory = client_factory.ClientFactory()
		gw = factory.create(id, self.__config)

		gw.set_params(self.__jsonReq.get_cel_number(), self.__jsonReq.get_provider(), proto.JSON_PACKET_NAME)
		gw.send_sms(self.__jsonReq.get_sms())
		self.__logger.info("otp sended: " + self.__jsonReq.get_sms())

		return 0


	def __service(self, data):
		self.__logger.debug("json packet req: " + data)
		
		# Converts a json string to a bidimensional array.
		self.__jsonReq = proto.JsonRequest(data)
		self.__jsonReq.str2json()
		self.__packetId = self.__jsonReq.get_id()

		i = 0
		for gwId in self.__gw_priority:
			i = i + 1

			try:
				ret = self.__exec_client(gwId)
				if ret == 0:
					break

			except Exception, e:
				self.__logger.error(e)
				if i == len(self.__gw_priority):
					raise client.ClientError("Couldn't send the sms.")

	def deliver_sms(self):
		jsonResp = proto.JsonResponse()
		jsonResp.set_ver = 1

		while 1:
			# Receives the data from pam module.
			conn, port = self.__sockd.accept()
			data = conn.recv(1024)
	
			try:
				# Sends sms from the client to the gateway.
				self.__service(data)
				
				jsonResp.set_id(self.__packetId)
				jsonResp.set_result(proto.JSON_RESP_OK)
				jsonResp.set_msg("Send ok.")
				jsonStr = jsonResp.json2str()

				conn.send(jsonStr)
				self.__logger.debug("json packet resp: " + jsonStr)

			except client.ClientError, e:
				# If it couldn't send the sms to any gateway, it sends the 
				# error to the pam module.
				self.__logger.error(e)
	
				jsonResp.set_id(self.__packetId)
				jsonResp.set_result(proto.JSON_RESP_ERR)
				jsonResp.set_msg("Send error.")
				jsonStr = jsonResp.json2str()

				conn.send(jsonStr)
				self.__logger.debug("json packet resp: " + jsonStr)

			except socket.error, e:
				self.__logger.error(e)

			conn.close

	def stop(self):
		self.__sockd.close()


if __name__ == "__main__":

	if len(sys.argv) != 2:
		sys.exit("Usage: ./server <sock_path>");

	try:
		srv = SMSAuthServer(sys.argv[1], SRV_CONF_FILE)
		srv.deliver_sms()

	except socket.error, e:
		print "socket.error: " + str(e)
		sys.exit(1)
	
	except KeyboardInterrupt:
		srv.stop()
		sys.exit(0)

