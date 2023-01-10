all:
	make -C real
	make -C complex

clean:
	make -C real clean
	make -C complex clean

fclean: 
	make -C real fclean
	make -C complex fclean

re: 
	make -C real re
	make -C complex re

.PHONY: all
.PHONY: clean
.PHONY: fclean
.PHONY: re