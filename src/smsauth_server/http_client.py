#!/usr/bin/python
#
# (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
#
import sys
import httplib
import urllib
import config
import client


HTTP_POST = "POST"
HTTP_GET = "GET"

HTTP_CONTENT_TYPE = "application/x-www-form-urlencoded"
HTTP_MIME = "text/html"


class HttpClient(client.Client):

	def __init__(self, confFile):
		try:
			self.__conf = config.ConfFileParser(confFile, config.CONF_HTTP_SECTION)
		except Exception, e:
			raise client.GwConfError("HTTP server.conf error: Section" + \
					" not found.")

	def send_sms(self, msg):
		self.__msg = msg
	
		headers = {}
		if self.__conf.get_http_method() == HTTP_POST:
			headers = {"Content-type": HTTP_CONTENT_TYPE, \
				 	"Accept": HTTP_MIME}
		#else self.__conf.get_http_method() == HTTP_GET:
		#	pass

		self.__set_url_params()

		conn = httplib.HTTPConnection(self.__conf.get_http_gw_addr())
		conn.request(self.__conf.get_http_method(), \
				self.__conf.get_http_script(), \
				self.__urlparams, headers)
		
		response = conn.getresponse()
		
		if response.status == 404:
			conn.close()
			raise client.HTTPClientError(response.status)

		elif response.status == 403:
			conn.close()
			raise client.HTTPClientError(response.status)

		elif response.status == 500:
			conn.close()
			raise client.HTTPClientError(response.status)

		data = response.read()
		conn.close()

	def set_params(self, celNumber, provider, fromName):
		self.__celNumber = celNumber
		self.__provider = provider
		self.__from = fromName

	def __set_url_params(self):
		dict = {}
		
		try:
			dict[self.__conf.get_http_var_cel()] = self.__celNumber
			dict[self.__conf.get_http_var_from()] = self.__from
			dict[self.__conf.get_http_var_provider()] = self.__provider
			dict[self.__conf.get_http_var_msg()] = self.__msg

		except Exception, e:
			raise client.GwConfError("HTTP server.conf error: Option not found.")

		self.__urlparams = urllib.urlencode(dict)


class HttpTestOk:

	def __init__(self):

		try:
			gw = HttpClient("server.conf")
			gw.set_params("2615175113", "cti", "smsauth")
			#gw.set_params("2615387138", "movistar", "smsauth")
			gw.send_sms("test HTTPClient")

		except HTTPClientError, e:
			print e
		except GwConfError, e:
			print e
		except Exception, e:
			print "Exception: " + str(e)


if __name__ == "__main__":
	
	HttpTestOk()

