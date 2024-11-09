all:
	@echo "=====Compiling inject====="
	gcc -g3 -o inject main.c
	@echo "=====Compiling woody ====="
	./inject ../resources/hello_world woody
	@chmod +x woody
	@echo "==========================\nResult: "
	# @./woody

clean:
	@rm -f inject woody

fclean: clean

re: fclean all