all: $(patsubst %.dot, %.png, $(wildcard *dot))

%.png: %.dot
	dot -Tpng $< > $@

clean:
	rm *.png