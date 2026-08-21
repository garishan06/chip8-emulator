CXX = /c/msys64/ucrt64/bin/g++.exe
CXXFLAGS = -I/c/msys64/ucrt64/include/SDL2
TARGET = emulator.exe
SOURCES = main.cpp chip8.cpp platform.cpp
LIBS = -lmingw32 -lSDL2main -lSDL2

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES) $(LIBS)

clean:
	rm -f $(TARGET)