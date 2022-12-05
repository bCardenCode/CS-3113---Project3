all: project3

ready: clean project3

project3: project3.c
	cc project3.c -o project3

clean:
	rm -f project3
