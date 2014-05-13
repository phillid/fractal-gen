all: config mbrot-gen raw-to-png raw-to-png-bw

mbrot-gen:
	- gcc -o mbrot-gen mbrot-gen.c ./config.so -lm -Wall

raw-to-png:
	- g++ -o raw-to-png raw-to-png.cpp -lm -lGL -lX11 -lpthread -Wall

raw-to-png-bw:
	- g++ -o raw-to-png-bw raw-to-png.cpp -DBW -lm -lGL -lX11 -lpthread -Wall


clean: clean-config clean-exec
clean-exec:
	- rm mbrot-gen raw-to-png -f

clean-config:
	- rm config.so -f

config:
	- gcc -shared -o config.so config.c


samples:
	- ./generate-samples
