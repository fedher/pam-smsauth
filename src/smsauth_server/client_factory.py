#!/usr/bin/python
#
# (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
#

import client
import http_client
import smtp_client
import mobile_client


class ClientFactory:
	
	def __init__(self):

		self.__client = {}

		self.__client["http"] = http_client.HttpClient
		self.__client["smtp"] = smtp_client.SmtpClient
		self.__client["mobile"] = mobile_client.MobileClient

	def create(self, type, arg=""):
	
		return self.__client[type](arg)



if __name__ == "__main__":
	import gammu

	factory = ClientFactory()
	#gw = factory.create("mobile", "server.conf")
	gw = factory.create("http", "server.conf")
	gw.set_params("2615175113", "cti", "smsauth")

	try:
		gw.send_sms("test Client Factory")
		
	except client.HTTPClientError, e:
		print e
		
	except client.GwConfError, e:
		print e

	except gammu.GSMError, e:
		print str(e)

	except Exception, e:
		print e

