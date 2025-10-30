CC:=gcc

num_endianess_convert: num_endianess_convert.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

inet_endian_convert: inet_endian_convert.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

single_conn_client: single_conn_client.c
	-$(CC) -o $@ $^

single_conn_server: single_conn_server.c
	-$(CC) -o $@ $^


without_buf_test: without_buf_test.c   
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

multi_conn_threads_client: multi_conn_threads_client.c
	-$(CC) -o $@ $^

multi_conn_threads_server: multi_conn_threads_server.c
	-$(CC) -o $@ $^


multi_conn_processes_client: multi_conn_processes_client.c
	-$(CC) -o $@ $^

multi_conn_processes_server: multi_conn_processes_server.c
	-$(CC) -o $@ $^


udp_server: udp_server.c
	-$(CC) -o $@ $^

udp_client: udp_client.c
	-$(CC) -o $@ $^

socket_ipc_test: socket_ipc_test.c
	-$(CC) -o $@ $^