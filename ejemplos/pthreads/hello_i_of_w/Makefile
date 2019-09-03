APPNAME = hello_i_of_w

$(APPNAME): $(APPNAME).c
	cc -g -Wall -Wextra $(APPNAME).c -o $(APPNAME) -pthread

.PHONY: clean	
clean:
	rm -f $(APPNAME)

#rules
#target: dependencias
#[tab]command
#make
# make clean
# ./hello_w >> jiji.txt : usar eso como salida sin sobrescribir
# ./hello_w > jiji.txt : usar como salida estandar SOBRESCRIBIENDO
#
#
