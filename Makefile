# 指定編譯器
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Iinclude -g
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# GLSL 編譯器
GLSLC = glslc

# 目標執行檔
TARGET = VulkanTest

# 搜尋所有 src/ 下的 .cpp 檔案
SRC = $(wildcard src/*.cpp) main.cpp
OBJ = $(SRC:.cpp=.o)

# Shader 檔案
SHADER_SRC = shaders/shader.vert shaders/shader.frag
SHADER_SPV = shaders/shader.vert.spv shaders/shader.frag.spv

# 編譯主目標
all: format $(SHADER_SPV) $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# 編譯 .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 編譯 GLSL -> SPIR-V
shaders/shader.vert.spv: shaders/shader.vert
	$(GLSLC) $< -o $@

shaders/shader.frag.spv: shaders/shader.frag
	$(GLSLC) $< -o $@

# 測試與清除
.PHONY: test clean format

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) $(SHADER_SPV)

# 格式化 C++ 程式碼
format:
	clang-format -i $(wildcard src/*.cpp) $(wildcard include/*.hpp) main.cpp
