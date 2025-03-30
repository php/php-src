FROM ubuntu:noble

RUN apt update && apt install -y pkg-config build-essential autoconf bison re2c \
                        libxml2-dev libsqlite3-dev cmake gdb
WORKDIR /app

CMD while true; do sleep 3600; done
