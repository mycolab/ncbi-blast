#!/usr/bin/env bash

[[ -z $PROJECT ]] && PROJECT=mycolab
export PROJECT

BASE_PATH=$1
[[ -z $BASE_PATH ]] && BASE_PATH="${HOME}/data/${PROJECT}/ncbi-blast"

if [[ ! -d $BASE_PATH ]];then
   echo "${BASE_PATH} is not a valid directory."
   exit 1
fi

blastdb_path="${BASE_PATH}/blastdb"
if [[ ! -d "${blastdb_path}" ]];then
    echo "creating blastdb directory: $blastdb_path"
    mkdir -p "${blastdb_path}"
else
    echo "blastdb directory: $blastdb_path"
fi

fasta_path="${BASE_PATH}/fasta"
if [[ ! -d "${fasta_path}" ]];then
    echo "creating fasta directory: $fasta_path"
    mkdir -p "${fasta_path}"
else
    echo "fasta directory: $fasta_path"
fi

./docker-net.sh || exit 1
docker compose -p $PROJECT up -d
