cc = gcc
target = test
include = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.c")
obj = $(src:%.c=%.o)

$(target) : $(obj)
	$(cc) -o $(target) $(obj)
%.o : %.c $(include)
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(target)