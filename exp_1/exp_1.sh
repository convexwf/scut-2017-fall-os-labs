#!/bin/bash
echo -e "\033[36m实验一-进程与线程\033[0m"
echo -e "\033[44m 1.正在配置make文件……\033[0m"
rm -rf ./bin/*
mkdir -p build && cd build/
echo -e "\033[44m 2.正在编译，连接(此阶段持续时间较长，请耐心等候）……\033[0m"
for file in `ls`
	do
	if [ "$file" != "Makefile" ];then
		rm -rf $file
	fi
done
`cmake ..`
make >/dev/null
cd ../bin
echo -e "\033[44m 3.程序成功生成\033[0m"
printf "\n*******************************************\n"
printf "请选择执行程序，按ENTER键继续，输入quit退出\n"
printf "\n可选程序有:\nan_ch2_1a\nan_ch2_1b\nnew_thread\n"
while true
do
	printf "\ninput:\t"
	read answer
	if [ "$answer" = "quit" ];then
		printf "程序已退出"
		break
	fi
	if [ "$answer" = "an_ch2_1a" ];then
		gnome-terminal -x bash -c "./an_ch2_1a"
	fi
	if [ "$answer" = "an_ch2_1b" ];then
		gnome-terminal -x bash -c "./an_ch2_1b"
	fi
	if [ "$answer" = "new_thread" ];then
		gnome-terminal -x bash -c "./new_thread"
	fi
done
