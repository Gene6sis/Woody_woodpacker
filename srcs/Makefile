srcs = 	main.c


all:
	@echo "=====Compiling inject====="
	gcc -g3 -o inject $(srcs)
	@echo "=====Compiling woody ====="
	./inject ../resources/hello_world woody
# @chmod +x woody
# @echo "==========================\nResult: "
# @./woody

clean:
	@rm -f inject woody

fclean: clean

re: fclean all