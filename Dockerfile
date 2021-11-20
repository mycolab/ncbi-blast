FROM ubuntu:latest

ENV DEBIAN_FRONTEND="noninteractive"

RUN apt clean

RUN apt-get update \
 && apt-get install -y build-essential git libtool-bin autopoint autotools-dev autoconf pkg-config \
    libncurses5-dev libncursesw5-dev gettext software-properties-common curl cpio

ADD . /app

WORKDIR /app

RUN ./configure

RUN make

RUN make install

CMD ["sleep", "86400"]
