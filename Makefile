default:
	mkdir -p build
	$(MAKE) runMultithreadMain
	$(MAKE) runMultiprocMain

runAatMain: cleanAatMain buildAat
	./build/aatMain

runCounterMain: cleanCounterMain buildCounter
	./build/counterMain text/HamletActISceneII.txt

runMultiprocMain: cleanMultiprocMain buildMultiproc buildSender
	./build/multiprocMain text/HamletActISceneII.txt text/HamletActIISceneI.txt text/HamletActIIISceneI.txt text/HamletActIIISceneII.txt text/HamletActIVSceneV.txt

runMultithreadMain: cleanMultithreadMain buildMultithread
	./build/multithreadMain text/HamletActISceneII.txt text/HamletActIISceneI.txt text/HamletActIIISceneI.txt text/HamletActIIISceneII.txt text/HamletActIVSceneV.txt

buildAat: build/main-aat.o build/aat.o build/stack.o build/strbuffer.o
	gcc build/main-aat.o build/aat.o build/stack.o build/strbuffer.o -o build/aatMain

buildCounter: build/main-counter.o build/counter.o build/aat.o build/stack.o build/strbuffer.o
	gcc build/main-counter.o build/counter.o build/aat.o build/stack.o build/strbuffer.o -o build/counterMain

buildMultiproc: build/main-multiproc.o buildSender
	gcc build/main-multiproc.o build/counter.o build/strbuffer.o build/aat.o build/stack.o -o build/multiprocMain

buildSender: build/main-sender.o
	gcc build/main-sender.o build/counter.o build/strbuffer.o build/aat.o build/stack.o -o build/senderMain

buildMultithread: build/main-multithread.o
	gcc build/main-multithread.o build/counter.o build/aat.o build/strbuffer.o build/stack.o -o build/multithreadMain

cleanAatMain:
	rm -f build/*.o build/*.d build/aatMain

cleanCounterMain:
	rm -f build/*.o build/*.d build/counterMain

cleanMultiprocMain:
	rm -f build/*{.o,.d,multiprocMain,senderMain}

cleanMultithreadMain:
	rm -f build/*{.o,.d,multithreadMain}

build/main-multithread.o: src/main-multithread.c build/counter.o build/aat.o
	gcc -c src/main-multithread.c -o build/main-multithread.o -Wall -O2 -MMD -MP

build/main-sender.o: src/main-sender.c build/counter.o
	gcc -c src/main-sender.c -o build/main-sender.o -Wall -O2 -MMD -MP

build/main-multiproc.o: src/main-multiproc.c build/counter.o build/strbuffer.o
	gcc -c src/main-multiproc.c -o build/main-multiproc.o -Wall -O2 -MMD -MP

build/main-counter.o: src/main-counter.c build/counter.o
	gcc -c src/main-counter.c -o build/main-counter.o -Wall -O2 -MMD -MP

build/counter.o: src/counter.c build/aat.o
	gcc -c src/counter.c -o build/counter.o -Wall -O2 -MMD -MP

build/main-aat.o: src/main-aat.c build/aat.o
	gcc -c src/main-aat.c -o build/main-aat.o -Wall -O2 -MMD -MP

build/aat.o: src/aat.c build/stack.o build/strbuffer.o
	gcc -c src/aat.c -Iinclude -o build/aat.o -Wall -O2 -MMD -MP

build/stack.o: src/stack.c
	gcc -c src/stack.c -Iinclude -o build/stack.o -Wall -O2 -MMD -MP

build/strbuffer.o: src/strbuffer.c
	gcc -c src/strbuffer.c -Iinclude -o build/strbuffer.o -Wall -O2 -MMD -MP

-include build/*.d