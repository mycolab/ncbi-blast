FROM ubuntu:latest
ENV DEBIAN_FRONTEND="noninteractive"

RUN apt clean
RUN apt-get update \
 && apt-get install -y build-essential git libtool-bin autopoint autotools-dev autoconf pkg-config \
    libncurses5-dev libncursesw5-dev gettext software-properties-common curl cpio

# compile and install blast
ADD . /blast
WORKDIR /blast
RUN ./configure
WORKDIR /blast/ReleaseMT/build
RUN make all_r
WORKDIR /blast
RUN make install

# install edirect tools
RUN perl -MNet::FTP -e '$ftp = new Net::FTP("ftp.ncbi.nlm.nih.gov", Passive => 1); $ftp->login; $ftp->binary; $ftp->get("/entrez/entrezdirect/edirect.tar.gz");'
RUN gunzip -c edirect.tar.gz | tar xf - && rm edirect.tar.gz && cp -r edirect/* /usr/local/bin

# import sequences and create a BLAST database
RUN mkdir blastdb queries fasta results

# move startup and update scripts to PATH
RUN chmod 755 *.sh
RUN mv *.sh /usr/local/bin

ENTRYPOINT ["/usr/local/bin/start-blast.sh"]
CMD ["--update", "--start-api"]
