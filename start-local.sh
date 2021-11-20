#!/usr/bin/env bash
export PROJECT=mycolab
./docker-net.sh

mkdir -p $HOME/data/mycolab/ncbi-blast/blastdb
mkdir -p $HOME/data/${PROJECT}/ncbi-blast/blastdb_custom
mkdir -p $HOME/data/${PROJECT}/ncbi-blast/fasta

docker compose -p $PROJECT up -d
