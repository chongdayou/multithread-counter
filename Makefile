default:
	$(MAKE) runAatMain

runAatMain: cleanAatMain buildAat
	./build/aatMain

buildAat: build/main-aat.o build/aat.o build/stack.o build/strbuffer.o
	gcc build/main-aat.o build/aat.o build/stack.o build/strbuffer.o -o build/aatMain

cleanAatMain:
	rm -f build/*.o build/*.d build/aatMain

build/main-aat.o: src/main-aat.c
	gcc -c src/main-aat.c -o build/main-aat.o -Wall -O2 -MMD -MP

build/aat.o: src/aat.c
	gcc -c src/aat.c -Iinclude -o build/aat.o -Wall -O2 -MMD -MP

build/stack.o: src/stack.c
	gcc -c src/stack.c -Iinclude -o build/stack.o -Wall -O2 -MMD -MP

build/strbuffer.o: src/strbuffer.c
	gcc -c src/strbuffer.c -Iinclude -o build/strbuffer.o -Wall -O2 -MMD -MP

-include build/*.d