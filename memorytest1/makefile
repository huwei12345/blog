memorytest:memorytest.o wintolinux.o buf_write_num.o getsize.o Malloc.o Read.o setcolor.o printferr.o file_lock.o
	gcc -Wall -g -fprofile-arcs -ftest-coverage -o memorytest wintolinux.o buf_write_num.o getsize.o Malloc.o Read.o setcolor.o memorytest.o printferr.o file_lock.o -lpthread -lgcov
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c -o test_main.o test_main.c
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c -o xtest.o xtest.c
	gcc -Wall -o test xtest.o test_main.o buf_write_num.o getsize.o Malloc.o Read.o setcolor.o printferr.o file_lock.o -lgcov
memorytest.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c memorytest.c
wintolinux.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c wintolinux.c -lpthread
printferr.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c printferr.c -lpthread
buf_write_num.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c buf_write_num.c
getsize.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c getsize.c
Malloc.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c Malloc.c
Read.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c Read.c
setcolor.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c setcolor.c
file_lock.o:
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c file_lock.c
clean: 
	rm -f *.o *.gcda *.gcno *.gcov demo.info
	rm -f *.yml
	rm -rf demo_web
	rm -f demo
	rm -f test

test: def
	./test --fork

check:
	valgrind --leak-check=full -v ./memorytest

lcov:
	lcov -d ./ -t 'memorytest' -o 'memorytest.info' -b . -c
	genhtml -o momorytest_web memorytest.info

.PHONY: def clean ut test
