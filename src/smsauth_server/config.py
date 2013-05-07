#!/usr/bin/python
#
# (C) 2006, J. Federico Hernandez <fede.hernandez@gmail.com>
#
import ConfigParser
import string


# Options
CONF_HTTP_SECTION = "HTTPgateway"
CONF_HTTP_METHOD = "http-method"
CONF_HTTP_GATEWAY = "http-gateway"
CONF_HTTP_SCRIPT = "http-script"
CONF_HTTP_VAR_MSG = "http-var-msg"
CONF_HTTP_VAR_CEL = "http-var-cel"
CONF_HTTP_VAR_PROVIDER = "http-var-provider"
CONF_HTTP_VAR_FROM = "http-var-from"

CONF_SMTP_SECTION = "SMTPgateway"
CONF_SMTP_LOGIN_USER = "smtp-login-user"
CONF_SMTP_LOGIN_PASS = "smtp-login-pass"
CONF_SMTP_GATEWAY = "smtp-gateway"
CONF_SMTP_PORT = "smtp-port"
CONF_SMTP_FROM = "smtp-from"
CONF_SMTP_SUBJECT = "smtp-subject"
CONF_SMTP_PROVIDER = "smtp-provider" 
CONF_SMTP_PROVIDER_DOMAIN = "smtp-provider-domain"

CONF_PRIORITY_SECTION = "Priority"
CONF_PRIORITY_GW = "gwClient"


class ConfFileParser:
	"""
	ConfFileParser: 
		Carga una seccio'n de un archivo de configuracio'n.
		Luego se pueden obtener los valores de las opciones que
		contiene la seccio'n.
	"""
	def __init__(self, fileName, section):

		self.parser = ConfigParser.ConfigParser()

		file = open(fileName)
		self.parser.readfp(file)
		
		self.parser.has_section(section)
		self.__section = section

	
	def get_items(self):
		"""
		Devuelve un conjunto de tuplas (opcion,valor) de una
		seccio'n.
		"""
		return self.parser.items(self.__section)


	def get_val4opt(self, option):
		"""
		Devuelve el valor para una opcio'n especificada.
		"""
		self.parser.has_option(self.__section, option)
		return self.parser.get(self.__section, option)

	def get_http_method(self):
		self.parser.has_option(self.__section, CONF_HTTP_METHOD)
		return self.parser.get(self.__section, CONF_HTTP_METHOD)
	
	def get_http_gw_addr(self):
		self.parser.has_option(self.__section, CONF_HTTP_GATEWAY)
		return self.parser.get(self.__section, CONF_HTTP_GATEWAY)

	def get_http_script(self):
		self.parser.has_option(self.__section, CONF_HTTP_SCRIPT)
		return self.parser.get(self.__section, CONF_HTTP_SCRIPT)

	def get_http_var_cel(self):
		self.parser.has_option(self.__section, CONF_HTTP_VAR_CEL)
		return self.parser.get(self.__section, CONF_HTTP_VAR_CEL)

	def get_http_var_msg(self):
		self.parser.has_option(self.__section, CONF_HTTP_VAR_MSG)
		return self.parser.get(self.__section, CONF_HTTP_VAR_MSG)

	def get_http_var_from(self):
		self.parser.has_option(self.__section, CONF_HTTP_VAR_FROM)
		return self.parser.get(self.__section, CONF_HTTP_VAR_FROM)

	def get_http_var_provider(self):
		self.parser.has_option(self.__section, CONF_HTTP_VAR_PROVIDER)
		return self.parser.get(self.__section, CONF_HTTP_VAR_PROVIDER)

	def get_smtp_login_usr(self):
		self.parser.has_option(self.__section, CONF_SMTP_LOGIN_USER)
		return self.parser.get(self.__section, CONF_SMTP_LOGIN_USER)

	def get_smtp_login_passwd(self):
		self.parser.has_option(self.__section, CONF_SMTP_LOGIN_PASS)
		return self.parser.get(self.__section, CONF_SMTP_LOGIN_PASS)

	def get_smtp_gw_addr(self):
		self.parser.has_option(self.__section, CONF_SMTP_GATEWAY)
		return self.parser.get(self.__section, CONF_SMTP_GATEWAY)

	def get_smtp_gw_port(self):
		self.parser.has_option(self.__section, CONF_SMTP_PORT)
		return int(self.parser.get(self.__section, CONF_SMTP_PORT))

	def get_smtp_from(self):
		self.parser.has_option(self.__section, CONF_SMTP_FROM)
		return self.parser.get(self.__section, CONF_SMTP_FROM)

	def get_smtp_subject(self):
		self.parser.has_option(self.__section, CONF_SMTP_SUBJECT)
		return self.parser.get(self.__section, CONF_SMTP_SUBJECT)

	def __get_smtp_provider(self):
		"""
		Return ['provider1', 'provider2', ...]
		"""
		self.parser.has_option(self.__section, CONF_SMTP_PROVIDER)
		return self.parser.get(self.__section, CONF_SMTP_PROVIDER)

	def __get_smtp_domain(self):
		"""
		Return ['domain1', 'domain2', ...]
		"""
		self.parser.has_option(self.__section, CONF_SMTP_PROVIDER_DOMAIN)
		return self.parser.get(self.__section, CONF_SMTP_PROVIDER_DOMAIN)

	def set_smtp_provider_domain(self):
		providers = self.__get_smtp_provider().split(",")
		domains = self.__get_smtp_domain().split(",")
		self.__domain = dict(zip(providers, domains))

	def get_smtp_providerDomain(self, provider):
		return self.__domain[provider]

	def get_gw_priority(self):
		self.parser.has_option(self.__section, CONF_PRIORITY_GW)
		priority = self.parser.get(self.__section, CONF_PRIORITY_GW)
		return priority.split(",")
		

class HttpTestOk:
	"""
	Test Ok: La seccion y la opcion si existen.
	"""
	def __init__(self, file):

		conf = ConfFileParser(file, CONF_HTTP_SECTION)
		print conf.get_items()
		print "Method: " + conf.get_val4opt("http-method")
		print "var-cel: " + conf.get_val4opt("http-var-cel")
		print conf.get_http_var_provider()


class HttpTestErr1:
	"""
	Test Error 1: La seccion no existe.
	"""
	def __init__(self, file):

		conf = ConfFileParser(file, "SMSgateway")
		print conf.get_items()
		print "Method: " + conf.get_val4opt("http-method")
	
class HttpTestErr2:
	"""
	Test Error 1: La opcion no existe.
	"""
	def __init__(self, file):

		conf = ConfFileParser(file, CONF_HTTP_SECTION)
		print conf.get_items()
		print "Method: " + conf.get_val4opt("http-dir")


class SmtpTestOk:
	"""
	Test Ok: La seccion y la opcion si existen.
	"""
	def __init__(self, file):

		conf = ConfFileParser(file, CONF_SMTP_SECTION)
		print conf.get_items()
		print "Gateway: " + conf.get_val4opt("smtp-gateway")
		print "Port: " + conf.get_val4opt("smtp-port")
		print "Login User: " + conf.get_smtp_login_usr()
		print "Login Pass: " + conf.get_smtp_login_passwd()
		conf.set_smtp_provider_domain()
		print "domain: " + conf.get_smtp_providerDomain("movistar")
		print "domain: " + conf.get_smtp_providerDomain("cti")

class PriorityTestOk:
	"""
	Test Ok: La seccion y la opcion si existen.
	"""
	def __init__(self, file):

		conf = ConfFileParser(file, CONF_PRIORITY_SECTION)
		priority = conf.get_gw_priority()
		for p in priority:
			print "priority: " + p


if __name__ == "__main__":

	try:
		SmtpTestOk("server.conf")
#		HttpTestOk("server.conf")
#		HttpTestOk("hola.txt")

#		HttpTestErr1("server.conf")
#		HttpTestErr2("server.conf")
#		PriorityTestOk("server.conf")
	except Exception, e:
		print "Error: " + str(e)
