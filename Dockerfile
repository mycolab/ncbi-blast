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
    # # Verify blast is working
    # RUN mkdir blastdb_custom
    # RUN efetch -db protein -format fasta -id P01349 > queries/P01349.fsa
    # RUN efetch -db protein -format fasta -id Q90523,P80049,P83981,P83982,P83983,P83977,P83984,P83985,P27950 > fasta/nurse-shark-proteins.fsa
    # WORKDIR /blast/blastdb_custom
    # RUN makeblastdb -in /blast/fasta/nurse-shark-proteins.fsa -dbtype prot -parse_seqids -out nurse-shark-proteins -title "Nurse shark proteins" -taxid 7801 -blastdb_version 5
    # RUN blastdbcmd -entry all -db nurse-shark-proteins -outfmt "%a %l %T" && ls -la
    # WORKDIR /blast

RUN ./update-data.sh

CMD ["sleep", "86400"]
