all:
	@echo "=====Compiling inject====="
	gcc -o inject main.c
	@echo "=====Compiling woody ====="
	./inject ../resources/hello_world woody
	@chmod +x woody
	@echo "==========================\nResult: "
	# @./woody

clean:
	@rm -f inject woody

fclean: clean

re: fclean all