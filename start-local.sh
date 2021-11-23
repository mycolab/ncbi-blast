#!/usr/bin/env bash

[[ -z $PROJECT ]] && PROJECT=mycolab
export PROJECT

./docker-net.sh || exit 1

mkdir -p "${HOME}/data/${PROJECT}/ncbi-blast/blastdb"
mkdir -p "${HOME}/data/${PROJECT}/ncbi-blast/fasta"

docker compose -p $PROJECT up -d
