FROM gcc:10.5

WORKDIR /app

RUN sed -i 's#http://deb.debian.org/#http://mirrors.aliyun.com/#' /etc/apt/sources.list
RUN set -e && apt update && apt install -y cmake make gnome-terminal
