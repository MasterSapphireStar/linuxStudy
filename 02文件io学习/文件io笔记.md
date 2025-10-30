
# 08fscanf
我先说明要做什么：解释为什么把 `fscanf` 放在循环里会读出三行，而只调用一次会只打印第一行，并给出安全且常用的正确写法示例。

核心原因（简短）
- 文件流有一个“当前位置”（file position）。每次调用 `fscanf` / `fgets` / `fread` 都会从当前位置读取数据并推进这个位置到下一个未读的位置。  
- 如果你只调用一次 `fscanf`，缓冲区只被填一次，后面再 `printf` 只是重复打印同一个缓冲区内容；把 `fscanf` 写在 `while` 条件或循环体里就会在每次迭代都重新从文件读取并更新缓冲区，因此可以依次打印文件里的每一行/每个记录。

示例：假设文件 `data.txt` 三行内容
```
line1
line2
line3
```

错误写法（只调用一次 fscanf）
```c
char buf[100];
fscanf(fp, "%99s", buf);   // 只读取一次
for (int i = 0; i < 3; ++i) {
    printf("%s\n", buf);   // 每次都打印同一个 buf —— line1 三次
}
```

正确写法（在循环中多次调用 fscanf，逐条读取）
```c
char buf[100];
while (fscanf(fp, "%99s", buf) == 1) {
    printf("%s\n", buf);   // 每次循环都会从文件读取下一个 token 并打印
}
```
上面 `while` 的每次迭代都会执行 `fscanf`，因此文件当前位置向后推进，最终输出：
```
line1
line2
line3
```

注意点与细节
- fscanf 返回的是成功匹配并赋值的字段数（或遇到 EOF 返回 EOF），因此用 `while (fscanf(...) == 1)` 或 `!= EOF`（针对返回 EOF 的旧式判断）来控制循环比较稳妥。不要用 `feof` 来控制输入循环（通常会导致处理最后一次读取失败的混乱逻辑）。
- `%s` 格式按空白分词（空格、制表、换行），所以它不会保留行内的空格；若你想读整行（包含空格），用 `fgets` 更合适，或者用 `fscanf(fp, "%99[^\n]", buf)`（注意处理换行）。
- 安全性：总是限定宽度以防缓冲区溢出，例如 `%99s` 对应 `char buf[100]`。
- 若需要处理任意长行，考虑 `getline`（自动扩容）或用 `realloc` 动态拼接。

fgets 的等价安全写法（按行读取）
```c
char buf[100];
while (fgets(buf, sizeof buf, fp)) {
    fputs(buf, stdout); // 会保留行尾的换行符
}
```

总结
- “只调用一次 `fscanf`”只是把文件里的第一个可匹配项读到缓冲区一次，所以打印的都是同一内容。  
- 把 `fscanf`（或 `fgets`）放在循环里，每次迭代都会从文件中继续读取下一段数据，因此能依次打印多行/多项。  
- 推荐使用返回值检查和宽度限制（如 `%99s`），并在需要按行读取时优先 `fgets` 或 `getline`。

如果你愿意，把你当前实际的代码贴出来（或我可以打开 08fscanf.c 看看），我可以针对你写法给出具体修正并帮你编译运行一次验证。要我继续吗？


# 09stdin_out_err
```c
#include <stdio.h>
#include <stdlib.h>

int main()
{
	char *ch = (char *)malloc(20);
	//从标准输入读取字符串
	fgets(ch, 20, stdin);
	printf("你输入的字符串是: %s\n", ch);
	//标准输出
	fputs(ch, stdout);
	//标准错误输出
	fputs(ch, stderr);
	free(ch);
	return 0;
}
```

