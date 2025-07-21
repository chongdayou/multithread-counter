default:
	$(MAKE) runCounterMain

runAatMain: cleanAatMain buildAat
	./build/aatMain

runCounterMain: cleanCounterMain buildCounter
	./build/counterMain

buildAat: build/main-aat.o build/aat.o build/stack.o build/strbuffer.o
	gcc build/main-aat.o build/aat.o build/stack.o build/strbuffer.o -o build/aatMain

buildCounter: build/main-counter.o build/counter.o build/aat.o build/stack.o build/strbuffer.o
	gcc build/main-counter.o build/counter.o build/aat.o build/stack.o build/strbuffer.o -o build/counterMain

cleanAatMain:
	rm -f build/*.o build/*.d build/aatMain

cleanCounterMain:
	rm -f build/*.o build/*.d build/counterMain

build/main-counter.o: src/main-counter.c
	gcc -c src/main-counter.c -o build/main-counter.o -Wall -O2 -MMD -MP

build/counter.o: src/counter.c
	gcc -c src/counter.c -o build/counter.o -Wall -O2 -MMD -MP

build/main-aat.o: src/main-aat.c
	gcc -c src/main-aat.c -o build/main-aat.o -Wall -O2 -MMD -MP

build/aat.o: src/aat.c
	gcc -c src/aat.c -Iinclude -o build/aat.o -Wall -O2 -MMD -MP

build/stack.o: src/stack.c
	gcc -c src/stack.c -Iinclude -o build/stack.o -Wall -O2 -MMD -MP

build/strbuffer.o: src/strbuffer.c
	gcc -c src/strbuffer.c -Iinclude -o build/strbuffer.o -Wall -O2 -MMD -MP

-include build/*.d