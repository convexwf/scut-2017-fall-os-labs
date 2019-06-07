#!/bin/bash
echo -e "\033[36m实验二——进程间通讯\033[0m"
echo -e "\033[44m 1.正在配置make文件……\033[0m"
rm -rf ./bin/*
cd build/
echo -e "\033[44m 2.正在编译，连接(此阶段持续时间较长，请耐心等候）……\033[0m"
for file in `ls`
	do
	if [ "$file" != "Makefile" ];then
		rm -rf $file
	fi
done
eval "cmake .."
eval "make >/dev/null"
cd ../bin
echo -e "\033[44m 3.程序成功生成\033[0m"
printf "\n*******************************************\n"
printf "请选择执行程序，按ENTER键继续，输入quit退出\n"
printf "\n可选程序有:\nproducer_consumer\nbarber\nreader_writer\n"
while true
do
	printf "\ninput:\t"

	read answer
	if [ "$answer" = "quit" ];then
		printf "程序已退出\n"
		eval "ipcs -s | grep 0X |cut -d" " -f2|xargs -n1 ipcrm -s"
		break
	fi
	if [ "$answer" = "producer_consumer" ];then
		gnome-terminal -x bash -c "./producer_consumer"
	fi
	if [ "$answer" = "barber" ];then
		gnome-terminal -x bash -c "./barber"
	fi
	if [ "$answer" = "reader_writer" ];then
		gnome-terminal -x bash -c "./reader_writer"
	fi

done
