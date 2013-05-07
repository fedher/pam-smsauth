#!/usr/bin/python
#
# (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
#
import sys
import smtplib
import config
import client


class SmtpClient(client.Client):

	def __init__(self, confFile):
		try:
			self.__conf = config.ConfFileParser(confFile, config.CONF_SMTP_SECTION)
			self.__conf.set_smtp_provider_domain()

		except Exception, e:
			raise client.GwConfError("SMTP server.conf error: Section" +
					" not found.")

	def send_sms(self, msg):
		fromAddr = self.__conf.get_smtp_from()
		to_addr = self.__to_addr()

		msg = "From: " + fromAddr + "\r\nTo: " + to_addr + "\r\n\r\n" + msg

		server = smtplib.SMTP(self.__conf.get_smtp_gw_addr(), self.__conf.get_smtp_gw_port())

		server.set_debuglevel(0)
		server.login(self.__conf.get_smtp_login_usr(), self.__conf.get_smtp_login_passwd())
		server.sendmail(fromAddr, to_addr, msg)
		server.quit()

	def __to_addr(self):
		return self.__cel_number + "@" + \
			self.__conf.get_smtp_providerDomain(self.__provider)

	def set_params(self, cel_number, provider, subject):
		self.__cel_number = cel_number
		self.__provider = provider
		self.__subject = subject


class SmtpTestOk:

	def __init__(self):
		try: 
			gw = SmtpClient("server.conf")
			gw.set_params("2615175113", "cti", "smsauth")
			#gw.set_params("2615387138", "movistar", "smsauth")
			gw.send_sms("test SMTPClient")

		except Exception, e:
			print e


if __name__ == "__main__":
	
	SmtpTestOk()

