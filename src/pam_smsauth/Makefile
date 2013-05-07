#
# (C) 2006, J. Federico Hernandez Scarso <fede.hernandez@gmail.com> 
#

# -fPIC: position-independent code
# (dynamic linking)

CFLAGS	= -fPIC -g -Wall 

# -shared: create a shared library
# -Wl:

LDFLAGS	= -shared -Wl 
LDLIBS	= -lpam -lc -lsqlite3
TARGETS	= pam_smsauth.so
DST_PATH = /lib/security/
CONF_DIR = /etc/smsauth
CONF_FILE = ../../conf/server.conf
LOG_DIR = /var/log/smsauth
REMOTE_PATH = root@shishapangma:$(DST_PATH)


all: $(TARGETS)


pam_smsauth.so: pam_smsauth.o pam_prompt.o pam_syslog.o otp.o user.o \
		json_proto.o sms.o error.o pam_parse_args.o pam_data.o \
		sqlite_client.o pam_register_var.o \
		json/json_object.o json/json_tokener.o json/json_util.o \
		json/arraylist.o json/debug.o json/linkhash.o json/printbuf.o

	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)


install: $(TARGETS)
	@echo "Installing in $(DST_PATH) ..."
	@test -d $(DST_PATH)
	@set -x;install -d $(DST_PATH) && install -p -m 644 $(TARGETS) $(DST_PATH)
	@echo "Creating $(CONF_DIR) $(LOG_DIR) ..."
	@sudo mkdir $(CONF_DIR) $(LOG_DIR)
	@sudo cp -v $(CONF_FILE) $(CONF_DIR)

rinstall: $(TARGETS)
	@echo "Installing in $(REMOTE_PATH) ..."
	@scp $(TARGETS) $(REMOTE_PATH)

uninstall:
	rm -fv $(DST_PATH)/$(TARGETS)
	rm -frv $(CONF_DIR) $(LOG_DIR)

clean:
	rm -fv *o *~ $(TARGETS) core*
	rm -fv $(DST_PATH)/$(TARGETS)
	rm -fv json/*.o


