FLAGS= -Wall -Werror -pedantic -std=c++11
AUTORELEASE_LIB= ReferenceCounted.o Autorelease.o

# creates a static version of the library to link locally
# Code that uses this library must include two flags
# 	-lrefcount and -L(path to directory of librefcount.a)
local: Autorelease.cpp ReferenceCounted.cpp
	g++ $(FLAGS) -c Autorelease.cpp ReferenceCounted.cpp
	ar rcs librefcount.a Autorelease.o ReferenceCounted.o

install: Autorelease.cpp ReferenceCounted.cpp
	g++ $(FLAGS) -fPIC -c Autorelease.cpp ReferenceCounted.cpp
	g++ -shared -Wl,-soname=librefcount.so.1 -o librefcount.so.1.0 Autorelease.o ReferenceCounted.o
	sudo cp librefcount.so.1.0 /usr/local/lib/librefcount.so.1.0
	sudo ldconfig
	sudo ln -sf /usr/local/lib/librefcount.so.1 /usr/local/lib/librefcount.so
	sudo cp refcount.h /usr/local/include/refcount
	sudo cp Autorelease.h /usr/local/include/Autorelease.h
	sudo cp ReferenceCounted.h /usr/local/include/ReferenceCounted.h
	@make -s clean

uninstall:
	sudo rm /usr/local/include/refcount
	sudo rm /usr/local/include/Autorelease.h
	sudo rm /usr/local/include/ReferenceCounted.h
	sudo rm /usr/local/lib/librefcount*
	sudo ldconfig

clean:
	rm Autorelease.o ReferenceCounted.o librefcount.*
