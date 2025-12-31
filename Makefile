# Makefile for 2048 Game
# 需要安装SFML库: sudo apt-get install libsfml-dev (Ubuntu/Debian)
#                 或 brew install sfml (macOS)

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

TARGET = 2048
SRCDIR = src
INCDIR = include
OBJDIR = obj

# 源文件
SOURCES = main.cpp \
          $(SRCDIR)/Board.cpp \
          $(SRCDIR)/Animator.cpp \
          $(SRCDIR)/Renderer.cpp \
          $(SRCDIR)/SaveManager.cpp \
          $(SRCDIR)/RankList.cpp \
          $(SRCDIR)/Menu.cpp \
          $(SRCDIR)/Game.cpp

# 目标文件（放在obj目录中）
OBJECTS = $(OBJDIR)/main.o \
          $(OBJDIR)/Board.o \
          $(OBJDIR)/Animator.o \
          $(OBJDIR)/Renderer.o \
          $(OBJDIR)/SaveManager.o \
          $(OBJDIR)/RankList.o \
          $(OBJDIR)/Menu.o \
          $(OBJDIR)/Game.o

# 默认目标
all: $(OBJDIR) $(TARGET)

# 创建obj目录
$(OBJDIR):
	@mkdir -p $(OBJDIR)
	@echo "创建目录: $(OBJDIR)"

# 链接
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "编译完成! 运行 './$(TARGET)' 或 'make run' 来启动游戏"

# 编译main.cpp
$(OBJDIR)/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译src目录下的源文件
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 运行游戏
run: $(TARGET)
	./$(TARGET)

# 清理
clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET)
	rm -f save.txt ranks.txt
	@echo "清理完成"

# 只删除目标文件，保留可执行文件
clean-obj:
	rm -rf $(OBJDIR)
	@echo "目标文件已删除"

# 重新编译
rebuild: clean all

.PHONY: all run clean clean-obj rebuild

