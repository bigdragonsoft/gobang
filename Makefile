# 编译器设置
CC = cc
CFLAGS = -Wall -Wextra -std=c99 -O2

# 目标可执行文件
TARGET = gobang

# 源文件
SRC = gobang.c

# 默认目标
all: $(TARGET)

# 编译规则
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

# 清理规则
clean:
	rm -f $(TARGET)

# 运行游戏
run: $(TARGET)
	./$(TARGET)

# 显示版本信息
version: $(TARGET)
	./$(TARGET) -v

# 伪目标声明
.PHONY: all clean run version