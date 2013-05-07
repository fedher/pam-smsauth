#!/usr/bin/python
#
# (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
#


class Client:

	def __init__(self, confFile):
		pass

	def send_sms(self, msg):
		pass

	def set_params(self, celNumber, provider, fromName):
		pass


class ClientError(Exception):

	def __init__(self, value):
		self.__value = value

	def __str__(self):
		return repr(self.__value)


class GwConfError(Exception):

	def __init__(self, value):
		self.__value = value

	def __str__(self):
		return repr(self.__value)


class HTTPClientError(Exception):

	def __init__(self, value):
		self.__status = value

	def __str__(self):
		if self.__status == 404:
			return repr("Error " + str(self.__status) + \
				": HTTP gateway not found. You must " + \
				"check the gateway configuration.")
		elif self.__status == 403:
			return repr("Error " + str(self.__status) + \
				": HTTP gateway forbidden. You must " + \
				"check the gateway configuration.")
		elif self.__status == 500:
			return repr("Error " + str(self.__status) + \
				": HTTP gateway internal error. You " + \
				"must check the gateway configuration.")

		return "Undefined error."

