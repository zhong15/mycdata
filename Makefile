profile =

# CC = cc -c
ifeq ($(profile), DEBUG)
CFLAGS = -std=c89 --debug -D DEBUG
else
CFLAGS = -std=c89
endif

# ABC = a b c
# $(ABC) = a b c

# all: main1.o main2.o
# all make 的时候可以同时执行 main1.o main2.o

# foo.o: bar.c baz.c
# $@ = foo.o（规则）
# $^ = bar.c baz.c（所有依赖）
# $< = bar.c（第一个依赖）

# .PHONY echoFoo
# make 每次执行都会判断文件是否最新，如果存在文件就不执行
# .PHONY 存在与规则同名文件的时候令不生成文件的规则仍能执行

all: a.o echoFoo

a.o: mycdata.o test.o
	$(CC) -o $@ $(CFLAGS) $^

test.o: test.c mycdata.h
	$(CC) -c $(CFLAGS) $^

mycdata.o: mycdata.c mycdata.h
	$(CC) -c $(CFLAGS) $^

.PHONY: clean echoFoo

clean:
	rm -f a.o test.o mycdata.o

echoFoo:
	echo FOO FOO FOO FOO
