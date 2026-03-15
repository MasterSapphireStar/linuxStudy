CC := gcc

system_test:system_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@


fork_test:fork_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

fork_fd_test:fork_fd_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

erlou: erlou.c
	-$(CC) -o $@ $^


execve_test:execve_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

fork_execve_test:fork_execve_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@


waitpid_test:waitpid_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

pstree_test:pstree_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

 
erlou_block: erlou_block.c
	-$(CC) -o $@ $^


orphan_process_test:orphan_process_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

perror_test:perror_test.c
	-$(CC) -o $@ $^
	-./$@
	-rm ./$@

errorno_test:errorno_test.c
	-$(CC) -o $@ $^
	-./$@ 
	-rm ./$@


unnamed_pipe_test:unnamed_pipe_test.c
	-$(CC) -o $@ $^
	-./$@ "test"
	-rm ./$@

process_test:process_test.c
	-$(CC) -o $@ $^
	-./$@ "test"
	-rm ./$@

fifo_write:fifo_write.c
	-$(CC) -o $@ $^

fifo_read:fifo_read.c
	-$(CC) -o $@ $^

shared_memory:shared_memory.c
	-$(CC) -o $@ $^
	-./$@ "test"
	-rm ./$@

shared_memory_block:shared_memory_block.c
	-$(CC) -o $@ $^
	-./$@ "test"
	-rm ./$@

father_son_mq_test:father_son_mq_test.c
	-$(CC) -o $@ $^
	-./$@ "test"
	-rm ./$@

producer:producer.c
	-$(CC) -o $@ $^

consumer:consumer.c
	-$(CC) -o $@ $^

signal_test:signal_test.c
	-$(CC) -o $@ $^
	-./$@ "test"
	-rm ./$@