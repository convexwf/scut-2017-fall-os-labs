#!/bin/bash
echo -e "\033[36m实验四——命令解释器\033[0m"
echo -e "\033[44m 1.正在配置make文件……\033[0m"
rm -rf ./bin/* ./build/*
cd build/
echo -e "\033[44m 2.正在编译，连接(此阶段持续时间较长，请耐心等候）……\033[0m"
for file in `ls`
	do
	if [ "$file" != "Makefile" ];then
		rm -rf $file
	fi
done
eval "cmake .."
eval "make"
cd ../bin
echo -e "\033[44m 3.程序成功生成\033[0m"
printf "\n*******************************************\n"

gnome-terminal -x bash -c "./QShell"

