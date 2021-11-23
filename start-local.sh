#!/usr/bin/env bash
export PROJECT=mycolab
./docker-net.sh

mkdir -p $HOME/data/${PROJECT}/ncbi-blast/blastdb
mkdir -p $HOME/data/${PROJECT}/ncbi-blast/fasta

docker compose -p $PROJECT up -d
