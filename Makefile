main:
	g++ main.cpp -o bin/main && (time ./bin/main > image.ppm) && xdg-open image.ppm
