CC:=gcc

01system: 01system.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

02fork: 02fork.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

03fork_fd: 03fork_fd.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

04erlou: 04erlou.c
	-$(CC) -o $@ $^
#	-./$@
#	-rm -f $@

05execve: 05execve.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

06fork_execve: 06fork_execve.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

07waitpid: 07waitpid.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

08pstree: 08pstree.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

09erlou_block: 09erlou_block.c
	-$(CC) -o $@ $^
#	-./$@
#	-rm -f $@

09orphan_process: 09orphan_process.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@
