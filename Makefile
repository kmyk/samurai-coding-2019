.PHONY: build run

build: a.out

a.out: compile.sh main.cpp
	./compile.sh

OFFICIAL = software-for-SamurAI-Coding-2018-19
TIMESTAMP := $(shell date +%s)

A = ./run.sh
# B = ${OFFICIAL}/player/greedy
B = ./bin/simple-beam-search
bench: build
	# unzip ${OFFICIAL}/samples/100-sample-courses.zip
	-mkdir log
	for i in $$(seq -w 00 99) ; do ${OFFICIAL}/official/official --stderrLogFile0 /dev/stderr ${OFFICIAL}/samples/courses/$$i.crs ${A} A ${B} B | tee log/${TIMESTAMP}.$$i.a.racelog | jq -r '.finished | @tsv' ; done
	for i in $$(seq -w 00 99) ; do ${OFFICIAL}/official/official --stderrLogFile1 /dev/stderr ${OFFICIAL}/samples/courses/$$i.crs ${B} B ${A} A | tee log/${TIMESTAMP}.$$i.b.racelog | jq -r '.finished | @tsv' ; done

score:
	cat log/${TIMESTAMP}.??.?.racelog | jq -r '.finished | @csv' | awk -F, '{ a += $$1 ; b += $$2 } END { print a, b }'

viewer:
	sensible-browser ${OFFICIAL}/viewer/viewer.html

archive:
	tar cf archive.tar.gz compile.sh run.sh main.cpp
