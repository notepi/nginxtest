
default:	build

clean:
	rm -rf Makefile objs

build:
	$(MAKE) -f objs/Makefile
	$(MAKE) -f objs/Makefile manpage

install:
	$(MAKE) -f objs/Makefile install

upgrade:
	/home/linux/share/servers/sbin/nginx -t

	kill -USR2 `cat /home/linux/share/servers/logs/nginx.pid`
	sleep 1
	test -f /home/linux/share/servers/logs/nginx.pid.oldbin

	kill -QUIT `cat /home/linux/share/servers/logs/nginx.pid.oldbin`
