#src = $(wildcard ./src/*.c)
#targets = $(patsubst %.c, %, $(src))

#CC = gcc
#CFLAGS = -Wall -g 
#all:$(targets)

#$(targets):%:%.c
#	$(CC) $< -o $@ $(CFLAGS) -I/Heard
	
	
obj=./src/*.c #这里的.o文件是不存在的，所以会向下查找依赖
target=iotapp
#下面的格式就是  目标：依赖     在重新编译时，目标的日期比依赖的日期旧，那就会重新生成
$(target):$(obj)
	gcc $(obj) -o $(target) -I Heard -g -lpthread -lmysqlclient

.PHONY:clean all
clean:
	-rm -rf $(target)
