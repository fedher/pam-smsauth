# !/usr/bin/python
#

import simplejson

JSON_PACKET_NAME = "pam_smsauth"

JSON_CMD_REQ = "REQ"
JSON_CMD_RESP = "RESP"
JSON_CMD = {JSON_CMD_REQ: 1, JSON_CMD_RESP: 2} 

JSON_RESP_OK = "OK"
JSON_RESP_ERR = "ERR"
JSON_RESP = {JSON_RESP_OK: 1, JSON_RESP_ERR: 2}


class JsonRequest:

	def __init__(self, jsonStr):
		self.__jsonStr = jsonStr

	def str2json(self):
		self.__packet = simplejson.loads(self.__jsonStr)
		self.__save_data()
	
	def __save_data(self):
		self.__id = int(self.__packet[JSON_PACKET_NAME]["id"])
		self.__ver = self.__packet[JSON_PACKET_NAME]["ver"]
		self.__cmd = self.__packet[JSON_PACKET_NAME]["cmd"]
		self.__provider = self.__packet[JSON_PACKET_NAME]["provider"]
		self.__cel = self.__packet[JSON_PACKET_NAME]["cel_number"]
		self.__sms = self.__packet[JSON_PACKET_NAME]["sms"]

	def get_id(self):
		return int(self.__id)

	def get_ver(self):
		return self.__ver

	def get_cmd(self):
		return self.__cmd

	def get_provider(self):
		return self.__provider

	def get_cel_number(self):
		return self.__cel

	def get_sms(self):
		return self.__sms

	def get_str(self):
		return self.__jsonStr


class JsonResponse:

	def __init__(self):
		self.__packetDict = {}
		self.__packetDict[JSON_PACKET_NAME] = {}
		self.set_cmd(JSON_CMD_RESP)

	def set_id(self, id):
		self.__packetDict[JSON_PACKET_NAME]["id"] = id

	def set_cmd(self, cmdIdx):
		self.__packetDict[JSON_PACKET_NAME]["cmd"] = JSON_CMD[cmdIdx]

	def set_result(self, resuIdx):
		self.__packetDict[JSON_PACKET_NAME]["result"]= JSON_RESP[resuIdx]

	def set_msg(self, msg):
		self.__packetDict[JSON_PACKET_NAME]["msg"] = msg

	def set_ver(self, ver):
		self.__packetDict[JSON_PACKET_NAME]["ver"] = ver

	def json2str(self):
		return simplejson.dumps(self.__packetDict, sort_keys=False)


if __name__ == "__main__":

		jsonReq = JsonRequest("{\"pam_smsauth\": {\"ver\": 1," + 
				" \"cmd\": 1, \"id\": 1, \"provider\": " + 
				"\"cti\", \"cel_number\": \"2615387138\"," +
				"\"sms\": \"hula\"}}")
		jsonReq.str2json()

		print "str: " + jsonReq.get_str()

		print "cel: " + jsonReq.get_cel_number()
		print "prov: " + jsonReq.get_provider()
		print "sms: " + jsonReq.get_sms()

		id = jsonReq.get_id()

		jsonResp = JsonResponse()
		jsonResp.set_ver = 1
		jsonResp.set_id(id + 1)

		jsonResp.set_result(JSON_RESP_OK)
		jsonResp.set_msg("Send ok.")
		jsonStr = jsonResp.json2str()
		print "JSON OK: " +  jsonStr

		jsonResp.set_result(JSON_RESP_ERR)
		jsonResp.set_msg("Send err.")
		jsonStr = jsonResp.json2str()
		print "JSON ERR: " +  jsonStr
