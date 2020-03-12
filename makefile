dic_test_upsize: dic_test_upsize.c dic_function.h dic_function.c
	gcc dic_test_upsize.c dic_function.h dic_function.c -o dic_test_upsize

clean:
	rm -rf dic dic_test_upsize