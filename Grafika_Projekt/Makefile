LIBS=-lGL -lglfw -lGLEW -lassimp
HEADERS=stb_image.h Shader.hpp Camera.hpp Mesh.hpp Model.hpp
FILES=main.cpp stb_image.cpp Shader.cpp Camera.cpp Mesh.cpp Model.cpp
# main: $(FILES) $(HEADERS)
compile: $(FILES) $(HEADERS)
	mkdir -p build && g++ -o build/main $(FILES)  $(LIBS) -Wall -I.
release:
	mkdir -p build && g++ -o build/main $(FILES)  $(LIBS) -O2 -I.
compile-run:
	mkdir -p build && g++ -o build/main $(FILES)  $(LIBS) -I. && ./build/main
release-run:
	mkdir -p build && g++ -o build/main $(FILES)  $(LIBS) -O2 -I. && ./build/main
run:
	./build/main
clean:
	rm build/*