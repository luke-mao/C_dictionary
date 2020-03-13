dic_test: dic_test.c dic_function.h dic_function.c
	gcc dic_test.c dic_function.h dic_function.c -o dic_test

clean:
	rm -rf dic_test