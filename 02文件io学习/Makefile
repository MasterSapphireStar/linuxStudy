CC:=gcc

01fopen_test: 01fopen_test.c
#gcc -o 01fopen_test 01fopen_test.c  -:忽略错误继续执行  $@表示目标文件 $^表示所有的依赖文件
	-$(CC) -o $@ $^
#./01fopen_test:执行当前目录下的01fopen_test文件
	-./$@
#rm -f ./01fopen_test:删除当前目录下的01fopen_test文件 -f:强制删除不提示
	-rm -f $@


02fclose_test: 02fclose_test.c
#gcc -o 02fclose_test 02fclose_test.c  -:忽略错误继续执行  $@表示目标文件 $^表示所有的依赖文件
	-$(CC) -o $@ $^
#./02fclose_test:执行当前目录下的02fclose_test文件
	-./$@
#rm -f ./02fclose_test:删除当前目录下的02fclose_test文件 -f:强制删除不提示
	-rm -f $@

03fputc: 03fputc.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

04fputs: 04fputs.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

05fprintf: 05fprintf.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

06fgetc: 06fgetc.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

07fgets: 07fgets.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

08fscanf: 08fscanf.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

09stdin_out_err: 09stdin_out_err.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

10unstd: 10unstd.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@

11system_call: 11system_call.c
	-$(CC) -o $@ $^
	-./$@
	-rm -f $@