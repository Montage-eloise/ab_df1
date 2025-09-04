# AB DF1 Library Makefile
# 这是一个简单的Makefile，用于不使用CMake的场合

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99 -Iinclude
LDFLAGS = 

# 目录
SRCDIR = src
INCDIR = include
EXAMPLEDIR = examples
TESTDIR = tests
BUILDDIR = build
LIBDIR = lib

# 源文件
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# 库文件
STATIC_LIB = $(LIBDIR)/libab_df1.a
SHARED_LIB = $(LIBDIR)/libab_df1.so

# 示例程序
EXAMPLES = $(BUILDDIR)/simple_read $(BUILDDIR)/simple_write $(BUILDDIR)/address_parser_demo

# 测试程序
TESTS = $(BUILDDIR)/test_address $(BUILDDIR)/test_protocol

# 默认目标
all: $(STATIC_LIB) $(SHARED_LIB) examples tests

# 创建目录
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

# 编译对象文件
$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# 静态库
$(STATIC_LIB): $(OBJECTS) | $(LIBDIR)
	ar rcs $@ $^

# 动态库
$(SHARED_LIB): $(OBJECTS) | $(LIBDIR)
	$(CC) -shared -o $@ $^ $(LDFLAGS)

# 示例程序
examples: $(EXAMPLES)

$(BUILDDIR)/simple_read: $(EXAMPLEDIR)/simple_read.c $(STATIC_LIB) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lab_df1

$(BUILDDIR)/simple_write: $(EXAMPLEDIR)/simple_write.c $(STATIC_LIB) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lab_df1

$(BUILDDIR)/address_parser_demo: $(EXAMPLEDIR)/address_parser_demo.c $(STATIC_LIB) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lab_df1

# 测试程序
tests: $(TESTS)

$(BUILDDIR)/test_address: $(TESTDIR)/test_address.c $(STATIC_LIB) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lab_df1

$(BUILDDIR)/test_protocol: $(TESTDIR)/test_protocol.c $(STATIC_LIB) | $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $< -L$(LIBDIR) -lab_df1

# 运行测试
test: tests
	@echo "运行地址解析测试..."
	@$(BUILDDIR)/test_address
	@echo ""
	@echo "运行协议测试..."
	@$(BUILDDIR)/test_protocol

# 清理
clean:
	rm -rf $(BUILDDIR) $(LIBDIR)

# 安装（需要root权限）
install: all
	install -d /usr/local/lib
	install -d /usr/local/include/ab_df1
	install -m 644 $(STATIC_LIB) /usr/local/lib/
	install -m 755 $(SHARED_LIB) /usr/local/lib/
	install -m 644 $(INCDIR)/ab_df1/*.h /usr/local/include/ab_df1/
	ldconfig

# 卸载
uninstall:
	rm -f /usr/local/lib/libab_df1.a
	rm -f /usr/local/lib/libab_df1.so
	rm -rf /usr/local/include/ab_df1

# 显示帮助
help:
	@echo "可用的目标:"
	@echo "  all       - 构建库和示例程序"
	@echo "  examples  - 构建示例程序"
	@echo "  tests     - 构建测试程序"
	@echo "  test      - 运行测试"
	@echo "  clean     - 清理构建文件"
	@echo "  install   - 安装库（需要root权限）"
	@echo "  uninstall - 卸载库（需要root权限）"
	@echo "  help      - 显示此帮助信息"

.PHONY: all examples tests test clean install uninstall help
