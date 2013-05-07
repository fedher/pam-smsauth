#!/usr/bin/python
#
# (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
#
import gammu
import client

class MobileClient(client.Client):

	def __init__(self, confFile):
		self.__sm = gammu.StateMachine()
		self.__sm.ReadConfig()
		self.__sm.Init()

	def send_sms(self, msg):
		message = {'Text': msg, 'SMSC': {'Location': 1}, 'Number': self.__cel}
		self.__sm.SendSMS(message)

	def set_params(self, celNumber, provider, fromName):
		self.__cel = celNumber


if __name__ == "__main__":

	try:
		client = MobileClient("server.conf")
		client.set_params("2615175113", "", "")
		client.send_sms("test MobileClient")

	except gammu.GSMError, e:
		print "mobileClient: " + str(e)

