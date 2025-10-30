# Makefile 内容通常由以下三部分组成：
# <目标名称>:<前置依赖>
#	 \t<需要执行的命令>

# 定义变量 objects
objects = hello.o main.o

# 例如：
# 目标是编译出 main文件，依赖hello.o 和 main.o 两个目标文件
# 编译命令是 gcc hello.o main.o -o main
main: $(objects)
	gcc $(objects) -o main

# 目标是编译出 main.o 文件，依赖 main.c 和 hello.h
# 编译命令是 gcc -c main.c -o main.o (-o main.o可以省略，默认生成 main.o)
# main.o: main.c hello.h
# 	gcc -c main.c
main.o: hello.h  #自动推导

# 目标是编译出 hello.o 文件，依赖 hello.c 和 hello.h
# 编译命令是 gcc -c hello.c -o hello.o (-o hello.o可以省略，默认生成 hello.o)
# hello.o: hello.c hello.h
# 	gcc -c hello.c
hello.o: hello.h  #自动推导

#申明伪目标
.PHONY: clean #假如文件列表中有与目标同名的文件，如果不申明伪目标，make会认为该目标已经是最新的，从而不会执行clean命令

# 清理编译生成的文件 前面加-忽略错误
clean:
	-rm -f main $(objects) 
