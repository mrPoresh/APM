__start__: obj __lines_for_space__ interp xmlinterp4config __plugin__
	export LD_LIBRARY_PATH="./libs"; ./interp | (echo; echo; cat)

obj:
	mkdir obj


__lines_for_space__:
	@echo
	@echo
	@echo
	@echo
	@echo


__plugin__:
	cd plugin; make


CPPFLAGS = -Wall -g -pedantic -std=c++17 -Iinc
LDFLAGS = -Wall




xmlinterp4config: obj/xmlinterp.o obj/main.o
	g++ ${LDFLAGS} -o xmlinterp4config obj/xmlinterp.o obj/main.o -lxerces-c

interp: obj/xmlinterp.o obj/main.o
	g++ ${LDFLAGS} -o interp obj/xmlinterp.o obj/main.o -ldl -lxerces-c

obj/xmlinterp.o: src/xmlinterp.cpp inc/xmlinterp.hh
	g++ -c ${CPPFLAGS} -o obj/xmlinterp.o src/xmlinterp.cpp

obj/main.o: src/main.cpp inc/AbstractInterp4Command.hh inc/AbstractScene.hh\
            inc/AbstractComChannel.hh inc/xmlinterp.hh inc/LibInterface.hh\
            inc/Set4LibInterfaces.hh
	g++ -c ${CPPFLAGS} -o obj/main.o src/main.cpp

doc:
	cd dox; make

clean:
	rm -f obj/* interp core*


clean_plugin:
	cd plugin; make clean

cleanall: clean
	cd plugin; make cleanall
	cd dox; make cleanall
	rm -f libs/*
	find . -name \*~ -print -exec rm {} \;

help:
	@echo
	@echo "  Lista podcelow dla polecenia make"
	@echo 
	@echo "        - (wywolanie bez specyfikacji celu) wymusza"
	@echo "          kompilacje i uruchomienie programu."
	@echo "  clean    - usuwa produkty kompilacji oraz program"
	@echo "  clean_plugin - usuwa plugin"
	@echo "  cleanall - wykonuje wszystkie operacje dla podcelu clean oraz clean_plugin"
	@echo "             oprocz tego usuwa wszystkie kopie (pliki, ktorych nazwa "
	@echo "             konczy sie znakiem ~)."
	@echo "  help  - wyswietla niniejszy komunikat"
	@echo
	@echo " Przykladowe wywolania dla poszczegolnych wariantow. "
	@echo "  make           # kompilacja i uruchomienie programu."
	@echo "  make clean     # usuwa produkty kompilacji."
	@echo
 