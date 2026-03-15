#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
源文件编码转换工具
功能：检测并转换目录下所有源码文件为 UTF-8 编码
"""

import argparse
import json
import os
import sys
from datetime import datetime
from pathlib import Path


# 支持的源码文件扩展名
SUPPORTED_EXTENSIONS = [
    # C/C++
    '.c', '.h', '.cpp', '.hpp', '.cc', '.cxx', '.c++', '.h++', '.hh', '.hxx', '.def',
    # Python
    '.py', '.pyw',
    # Java
    '.java', '.jsp', '.jspx',
    # JavaScript/TypeScript
    '.js', '.jsx', '.ts', '.tsx', '.mjs', '.cjs',
    # Go
    '.go',
    # Rust
    '.rs',
    # Ruby
    '.rb', '.erb',
    # PHP
    '.php', '.php3', '.php4', '.php5', '.phtml',
    # C#
    '.cs',
    # Visual Basic
    '.vb', '.vbs',
    # Swift
    '.swift',
    # Kotlin
    '.kt', '.kts',
    # SQL
    '.sql',
    # Shell
    '.sh', '.bash', '.zsh', '.fish',
    # Lua
    '.lua',
    # Perl
    '.pl', '.pm', '.t',
    # R
    '.r', '.R',
    # Scala
    '.scala', '.sc',
    # Groovy
    '.groovy', '.gvy',
]

# 待检测的编码格式（按检测顺序）
ENCODINGS_TO_DETECT = ['utf-8', 'gb2312', 'gbk', 'gb18030', 'big5', 'latin-1', 'cp1252']

# 需要转换的非 UTF-8 编码
NON_UTF8_ENCODINGS = ['gb2312', 'gbk', 'gb18030', 'big5', 'latin-1', 'cp1252']

DEFAULT_LOG_FILE = ".encoding_convert_log.json"


def has_utf8_bom(file_path):
    """
    检测文件是否有 UTF-8 BOM (EF BB BF)
    返回：True/False
    """
    try:
        with open(file_path, 'rb') as f:
            bom = f.read(3)
            return bom == b'\xef\xbb\xbf'
    except Exception:
        return False


def detect_encoding(file_path):
    """
    检测文件编码
    返回：(编码格式，是否包含中文)
    """
    chinese_chars = []
    
    for enc in ENCODINGS_TO_DETECT:
        try:
            with open(file_path, 'r', encoding=enc) as f:
                content = f.read()
            
            # 检测是否包含中文字符
            has_chinese = any('\u4e00' <= c <= '\u9fff' for c in content)
            
            # 记录一些中文字符用于验证
            if has_chinese:
                chinese_chars = [c for c in content if '\u4e00' <= c <= '\u9fff'][:10]
            
            return enc, has_chinese, chinese_chars
        except (UnicodeDecodeError, UnicodeError, LookupError):
            continue
    
    return None, False, []


def convert_file(file_path, from_encoding, to_encoding='utf-8'):
    """
    转换文件编码
    返回：(成功与否，错误信息)
    """
    try:
        # 读取原文件
        with open(file_path, 'r', encoding=from_encoding) as f:
            content = f.read()
        
        # 写入新编码
        with open(file_path, 'w', encoding=to_encoding) as f:
            f.write(content)
        
        return True, None
    except Exception as e:
        return False, str(e)


def load_log(log_file):
    """加载转换记录"""
    if not os.path.exists(log_file):
        return {"converted_files": [], "timestamp": None}
    try:
        with open(log_file, 'r', encoding='utf-8') as f:
            return json.load(f)
    except Exception as e:
        print(f"警告：读取记录文件失败 - {e}")
        return {"converted_files": [], "timestamp": None}


def save_log(log_file, log_data):
    """保存转换记录"""
    log_data["timestamp"] = datetime.now().isoformat()
    try:
        with open(log_file, 'w', encoding='utf-8') as f:
            json.dump(log_data, f, indent=2, ensure_ascii=False)
        return True
    except Exception as e:
        print(f"错误：保存记录文件失败 - {e}")
        return False


def find_source_files(directory):
    """查找目录下所有支持的源码文件"""
    files = []
    for ext in SUPPORTED_EXTENSIONS:
        files.extend(directory.rglob(f'*{ext}'))
    return sorted(files)


def get_python_test_content():
    """返回 Python 测试文件内容"""
    return r'''#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
这是多行中文注释
测试文件编码转换
"""

# 单行中文注释
作者 = "张三"
城市 = '北京'
问候 = "你好，世界！"

# 字符串赋值测试
姓名 = "李四"
消息 = "欢迎使用编码转换工具"
路径 = "C:\\用户\\文档\\测试文件.txt"
引用文本 = "他说：'今天天气真好'"
混合内容 = "中文 + English + 数字 123 + 符号 @#$"

# 打印输出测试
print("打印中文内容")
print(f"格式化输出：{姓名} 来自 {城市}")
print("特殊字符：引号、冒号、分号；感叹号！问号？")

# 数据结构中的中文
用户信息 = {
    "姓名": "王五",
    "年龄": 25,
    "城市": "上海",
    "爱好": ["读书", "游泳", "编程"]
}

水果列表 = ["苹果", "香蕉", "橙子", "葡萄"]
坐标 = {"北京": (39.9, 116.4), "上海": (31.2, 121.5)}

# 函数定义
def 处理数据 (数据):
    """这是函数的中文文档字符串"""
    print(f"处理数据：{数据}")
    return 数据

def greet(name):
    # 中文注释：打招呼
    print(f"你好，{name}！")
    return "问候完成"

# 类定义
class 用户:
    """用户类，用于测试中文类名"""
    
    def __init__(self, name, age):
        self.name = name
        self.age = age
    
    def 自我介绍 (self):
        """返回自我介绍"""
        return f"我是{self.name}，今年{self.age}岁"

# 测试执行
if __name__ == "__main__":
    print("=" * 50)
    print("编码转换测试文件 - Python")
    print("=" * 50)

    处理数据 (用户信息)
    greet(姓名)

    用户对象 = 用户 ("赵六", 30)
    print(用户对象.自我介绍 ())
    
    print("\n所有中文内容测试完成！")
'''


def get_c_test_content():
    """返回 C 语言测试文件内容"""
    return r'''/*
 * 多行中文注释
 * 测试文件编码转换 - C 语言
 */

#include <stdio.h>
#include <string.h>

// 单行中文注释
const char* 作者 = "张三";
const char* 城市 = "北京";

// 中文结构体
typedef struct {
    const char* 姓名;
    int 年龄;
    const char* 城市;
} 用户信息;

// 中文函数名
void 打印问候 () {
    printf("你好，世界！\n");
    printf("欢迎使用编码转换工具\n");
}

void 处理用户 (用户信息 user) {
    // 打印用户信息
    printf("姓名：%s\n", user.姓名);
    printf("年龄：%d\n", user.年龄);
    printf("城市：%s\n", user.城市);
}

int main() {
    printf("========================================\n");
    printf("编码转换测试文件 - C 语言\n");
    printf("========================================\n");
    
    // 中文字符串
    const char* 消息 = "这是一个中文消息";
    const char* 特殊字符 = "引号\"冒号：分号；感叹号！问号？";
    
    printf("消息：%s\n", 消息);
    printf("特殊字符：%s\n", 特殊字符);
    
    // 结构体使用
    用户信息 user = {"李四", 25, "上海"};
    打印问候 ();
    处理用户 (user);
    
    printf("\n所有中文内容测试完成！\n");
    return 0;
}
'''


def get_java_test_content():
    """返回 Java 测试文件内容"""
    return r'''/**
 * 多行中文注释
 * 测试文件编码转换 - Java
 */

// 单行中文注释
public class 编码测试 {
    
    // 中文字段
    private static String 作者 = "张三";
    private static String 城市 = "北京";
    
    /**
     * 中文方法名
     * @param 名字 输入的名字
     */
    public static void 打印问候 (String 名字) {
        System.out.println("你好，" + 名字 + "！");
        System.out.println("欢迎使用编码转换工具");
    }
    
    public static void 处理数据 (String 数据) {
        // 中文注释
        System.out.println("处理数据：" + 数据);
    }
    
    public static void main(String[] args) {
        System.out.println("========================================");
        System.out.println("编码转换测试文件 - Java");
        System.out.println("========================================");
        
        // 中文字符串
        String 消息 = "这是一个中文消息";
        String 特殊字符 = "引号\"冒号：分号；感叹号！问号？";
        String 混合内容 = "中文 + English + 数字 123";
        
        System.out.println("消息：" + 消息);
        System.out.println("特殊字符：" + 特殊字符);
        System.out.println("混合内容：" + 混合内容);
        
        // 数组中的中文
        String[] 水果 = {"苹果", "香蕉", "橙子", "葡萄"};
        System.out.print("水果列表：");
        for (String f : 水果) {
            System.out.print(f + " ");
        }
        System.out.println();
        
        打印问候 ("李四");
        处理数据 ("测试数据");
        
        System.out.println("\n所有中文内容测试完成！");
    }
}
'''


def get_js_test_content():
    """返回 JavaScript 测试文件内容"""
    return r'''/**
 * 多行中文注释
 * 测试文件编码转换 - JavaScript
 */

// 单行中文注释
const 作者 = "张三";
const 城市 = "北京";

// 中文对象
const 用户信息 = {
    姓名："李四",
    年龄：25,
    城市："上海",
    爱好：["读书", "游泳", "编程"]
};

// 中文函数
function 打印问候 (名字) {
    console.log(`你好，${名字}！`);
    console.log("欢迎使用编码转换工具");
}

function 处理数据 (数据) {
    // 中文注释
    console.log(`处理数据：${数据}`);
}

// 中文字符串
const 消息 = "这是一个中文消息";
const 特殊字符 = '引号"冒号：分号；感叹号！问号？';
const 混合内容 = "中文 + English + 数字 123 + 符号 @#$";

console.log("========================================");
console.log("编码转换测试文件 - JavaScript");
console.log("========================================");

console.log("消息：" + 消息);
console.log("特殊字符：" + 特殊字符);
console.log("混合内容：" + 混合内容);

// 数组遍历
const 水果列表 = ["苹果", "香蕉", "橙子", "葡萄"];
console.log("水果列表：" + 水果列表.join(", "));

打印问候 ("王五");
处理数据 ("测试数据");

console.log("\n所有中文内容测试完成！");
'''


def create_test_files(output_dir):
    """
    创建各种编码格式的测试文件，包含丰富的中文使用场景
    """
    output_path = Path(output_dir).resolve()
    output_path.mkdir(parents=True, exist_ok=True)
    
    # 测试文件配置
    test_configs = [
        # Python 文件
        {'filename': 'test_utf8.py', 'content': get_python_test_content(), 'encoding': 'utf-8'},
        {'filename': 'test_utf8_bom.py', 'content': get_python_test_content(), 'encoding': 'utf-8-sig'},
        {'filename': 'test_gb2312.py', 'content': get_python_test_content(), 'encoding': 'gb2312'},
        {'filename': 'test_gbk.py', 'content': get_python_test_content(), 'encoding': 'gbk'},
        {'filename': 'test_gb18030.py', 'content': get_python_test_content(), 'encoding': 'gb18030'},
        # C 文件
        {'filename': 'test_utf8.c', 'content': get_c_test_content(), 'encoding': 'utf-8'},
        {'filename': 'test_utf8_bom.c', 'content': get_c_test_content(), 'encoding': 'utf-8-sig'},
        {'filename': 'test_gb2312.c', 'content': get_c_test_content(), 'encoding': 'gb2312'},
        {'filename': 'test_gbk.c', 'content': get_c_test_content(), 'encoding': 'gbk'},
        {'filename': 'test_gb18030.c', 'content': get_c_test_content(), 'encoding': 'gb18030'},
        # Java 文件
        {'filename': '编码测试_utf8.java', 'content': get_java_test_content(), 'encoding': 'utf-8'},
        {'filename': '编码测试_utf8_bom.java', 'content': get_java_test_content(), 'encoding': 'utf-8-sig'},
        {'filename': '编码测试_gb2312.java', 'content': get_java_test_content(), 'encoding': 'gb2312'},
        {'filename': '编码测试_gbk.java', 'content': get_java_test_content(), 'encoding': 'gbk'},
        {'filename': '编码测试_gb18030.java', 'content': get_java_test_content(), 'encoding': 'gb18030'},
        # JavaScript 文件
        {'filename': 'test_utf8.js', 'content': get_js_test_content(), 'encoding': 'utf-8'},
        {'filename': 'test_utf8_bom.js', 'content': get_js_test_content(), 'encoding': 'utf-8-sig'},
        {'filename': 'test_gb2312.js', 'content': get_js_test_content(), 'encoding': 'gb2312'},
        {'filename': 'test_gbk.js', 'content': get_js_test_content(), 'encoding': 'gbk'},
        {'filename': 'test_gb18030.js', 'content': get_js_test_content(), 'encoding': 'gb18030'},
    ]

    print(f"在目录创建测试文件：{output_path}")
    print("-" * 60)
    
    created_count = 0
    for config in test_configs:
        file_path = output_path / config['filename']
        try:
            with open(file_path, 'w', encoding=config['encoding']) as f:
                f.write(config['content'])
            print(f"  ✓ 已创建：{config['filename']} (编码：{config['encoding']})")
            created_count += 1
        except Exception as e:
            print(f"  ✗ 创建失败：{config['filename']} - {e}")
    
    print("-" * 60)
    print(f"完成！共创建 {created_count}/{len(test_configs)} 个测试文件")
    print(f"测试文件位置：{output_path}")
    print("\n提示：使用以下命令测试转换功能：")
    print(f"  python src_file_tools.py -d {output_path} --dry-run")
    print(f"  python src_file_tools.py -d {output_path}")


def main():
    parser = argparse.ArgumentParser(
        description='源文件编码转换工具 - 检测并转换目录下所有源码文件为 UTF-8 编码',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例:
  %(prog)s -d ./source                    转换 ./source 下所有源码文件
  %(prog)s -d ./source --dry-run          仅检测，不实际转换
  %(prog)s -d ./source --log my.json      指定记录文件
  %(prog)s --create-test ./test_files     创建各种编码的测试文件
        """
    )

    parser.add_argument('-d', '--directory', help='目标目录路径')
    parser.add_argument('--dry-run', action='store_true', 
                        help='仅检测并报告，不进行实际转换')
    parser.add_argument('--log', metavar='FILE',
                        help=f'记录文件路径 (默认：{DEFAULT_LOG_FILE})')
    parser.add_argument('--create-test', metavar='DIR',
                        help='创建各种编码格式的测试文件到指定目录')

    args = parser.parse_args()

    # 创建测试文件模式
    if args.create_test:
        create_test_files(args.create_test)
        return

    # 检查是否指定了目录
    if not args.directory:
        parser.print_help()
        print("\n错误：请指定目标目录 (-d/--directory) 或使用 --create-test 创建测试文件")
        sys.exit(1)

    directory = Path(args.directory).resolve()

    if not directory.exists():
        print(f"错误：目录不存在：{directory}")
        sys.exit(1)

    if not directory.is_dir():
        print(f"错误：不是目录：{directory}")
        sys.exit(1)

    mode_str = "[DRY RUN] " if args.dry_run else ""
    print(f"{mode_str}目标目录：{directory}")
    if args.log:
        print(f"{mode_str}记录文件：{args.log}")
    print("-" * 60)

    # 查找所有源码文件
    files = find_source_files(directory)

    if not files:
        print("未找到支持的源码文件")
        print(f"支持的扩展名：{', '.join(SUPPORTED_EXTENSIONS[:10])} ... 等")
        return

    print(f"找到 {len(files)} 个源码文件\n")

    # 加载现有记录（如果指定了日志）
    log_data = None
    if args.log:
        log_data = load_log(args.log)

    # 统计
    utf8_count = 0
    need_convert = []
    skipped = 0
    failed = 0

    # 检测每个文件
    for file_path in files:
        # 先检查是否有 UTF-8 BOM
        if has_utf8_bom(file_path):
            encoding = 'utf-8-sig'
            has_chinese = False
            chinese_chars = []
            # 用 utf-8-sig 读取内容检测中文
            try:
                with open(file_path, 'r', encoding='utf-8-sig') as f:
                    content = f.read()
                has_chinese = any('\u4e00' <= c <= '\u9fff' for c in content)
                if has_chinese:
                    chinese_chars = [c for c in content if '\u4e00' <= c <= '\u9fff'][:10]
            except Exception:
                pass
        else:
            encoding, has_chinese, chinese_chars = detect_encoding(file_path)

        if not encoding:
            skipped += 1
            print(f"  ⚠ 无法识别编码，跳过：{file_path}")
            continue

        chinese_info = " (含中文)" if has_chinese else ""

        if encoding.lower() == 'utf-8':
            utf8_count += 1
            print(f"  ✓ 已是 UTF-8{chinese_info}: {file_path}")
        elif encoding.lower() == 'utf-8-sig':
            need_convert.append((file_path, encoding, has_chinese, chinese_chars))
            print(f"  → 需要转换：{file_path} (utf-8-sig -> utf-8, 移除BOM){chinese_info}")
        else:
            need_convert.append((file_path, encoding, has_chinese, chinese_chars))
            chinese_sample = ""
            if chinese_chars:
                chinese_sample = f" 中文样本：{''.join(chinese_chars[:5])}"
            print(f"  → 需要转换：{file_path} ({encoding} -> UTF-8){chinese_info}{chinese_sample}")

    print(f"\n检测结果:")
    print(f"  已是 UTF-8: {utf8_count} 个")
    print(f"  需要转换：{len(need_convert)} 个")
    print(f"  跳过：{skipped} 个")

    # 执行转换
    if not args.dry_run and need_convert:
        print(f"\n开始转换...")
        converted = []
        
        for file_path, from_enc, has_chinese, _ in need_convert:
            success, error = convert_file(file_path, from_enc, 'utf-8')
            if success:
                converted.append(str(file_path.resolve()))
                print(f"  ✓ 已转换：{file_path} ({from_enc} -> UTF-8)")
            else:
                failed += 1
                print(f"  ✗ 转换失败：{file_path} - {error}")
        
        # 保存记录
        if args.log and converted:
            if log_data is None:
                log_data = {"converted_files": [], "timestamp": None}
            log_data["converted_files"] = list(set(log_data["converted_files"] + converted))
            if save_log(args.log, log_data):
                print(f"\n✓ 已记录 {len(converted)} 个转换的文件到：{args.log}")
        
        print("-" * 60)
        print(f"完成！转换：{len(converted)}, 失败：{failed}")
    elif args.dry_run and need_convert:
        print(f"\n[DRY RUN] 未执行实际转换")
        print(f"提示：移除 --dry-run 选项以执行转换")


if __name__ == '__main__':
    main()
