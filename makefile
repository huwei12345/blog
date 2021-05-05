memorytest:main1.o mysqlpool.o Field.o QueryResult.o CMysql.o MysqlManager.o addjson.o
	g++ -Wall -g -fprofile-arcs -ftest-coverage -o main1 main1.o mysqlpool.o CMysql.o Field.o QueryResult.o MysqlManager.o addjson.o cJSON.c -lpthread -lgcov -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c -o test_main.o test_main.c
	gcc -Wall -g -fprofile-arcs -ftest-coverage -c -o xtest.o xtest.c
	g++ -Wall -g -o test xtest.o test_main.o mysqlpool.o CMysql.o Field.o QueryResult.o MysqlManager.o addjson.o cJSON.c -lgcov -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
main1.o:
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c main1.cpp -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
mysqlpool.o:
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c mysqlpool.cpp -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
Field.o:
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c ./mysql/Field.cpp -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
QueryResult.o:
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c ./mysql/QueryResult.cpp -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
CMysql.o:
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c ./mysql/CMysql.cpp -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
MysqlManager.o:
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c MysqlManager.cpp -I /usr/include/mysql -L /usr/lib64/mysql -lmysqlclient
addjson.o:
	g++ -Wall -g -fprofile-arcs -ftest-coverage -c addjson.cpp cJSON.c -lm
clean: 
	rm -f *.o *.gcda *.gcno *.gcov demo.info
	rm -f *.yml
	rm -rf demo_web
	rm -f demo
	rm -f test

test: def
	./test --fork

check:
	valgrind --leak-check=full -v ./main1

lcov:
	lcov -d ./ -t 'main1' -o 'main1.info' -b . -c
	genhtml -o main1_web main1.info

.PHONY: def clean ut test
