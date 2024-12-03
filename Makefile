.PHONY: __start__ obj __lines_for_space__ __plugin__ doc clean clean_plugin cleanall help

__start__: obj __lines_for_space__ interp xmlinterp4config __plugin__
	LD_LIBRARY_PATH="./libs:$$LD_LIBRARY_PATH" ./interp | (echo; echo; cat)

obj:
	mkdir -p obj

__lines_for_space__:
	@printf "\n%.0s" {1..5}

__plugin__:
	$(MAKE) -C plugin || exit 1

CPPFLAGS = -Wall -g -pedantic -std=c++17 -Iinc
LDFLAGS = -Wall

xmlinterp4config: obj/xmlinterp.o obj/main.o obj/ProgramInterpreter.o
	g++ ${LDFLAGS} -o xmlinterp4config obj/xmlinterp.o obj/main.o obj/ProgramInterpreter.o -lxerces-c

interp: obj/xmlinterp.o obj/main.o obj/ProgramInterpreter.o
	g++ ${LDFLAGS} -o interp obj/xmlinterp.o obj/main.o obj/ProgramInterpreter.o -ldl -lxerces-c

obj/xmlinterp.o: src/xmlinterp.cpp inc/xmlinterp.hh | obj
	g++ -c ${CPPFLAGS} -o obj/xmlinterp.o src/xmlinterp.cpp

obj/ProgramInterpreter.o: src/ProgramInterpreter.cpp inc/ProgramInterpreter.hh
	g++ -c ${CPPFLAGS} -o obj/ProgramInterpreter.o src/ProgramInterpreter.cpp

obj/main.o: src/main.cpp inc/AbstractInterp4Command.hh inc/AbstractScene.hh\
            inc/AbstractComChannel.hh inc/xmlinterp.hh inc/LibInterface.hh\
            inc/Set4LibInterfaces.hh inc/Sender.hh inc/Cuboid.hh inc/Scene.hh | obj
	g++ -c ${CPPFLAGS} -o obj/main.o src/main.cpp

doc:
	$(MAKE) -C dox || exit 1

clean:
	rm -f obj/* interp xmlinterp4config core*

clean_plugin:
	$(MAKE) -C plugin clean || exit 1

cleanall: clean clean_plugin
	rm -f libs/*
	find . -name \*~ -exec rm {} \;
	find . -name \*.o -exec rm {} \;
	rm -f xmlinterp4config interp

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

