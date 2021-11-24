#!/usr/bin/env bash

BASE_PATH=$1
[[ -z $BASE_PATH ]] && BASE_PATH=${HOME}

if [[ ! -d $BASE_PATH ]];then
   echo "${BASE_PATH} is not a valid directory."
   exit 1
fi

[[ -z $PROJECT ]] && PROJECT=mycolab
export PROJECT

blastdb_path="${BASE_PATH}/data/${PROJECT}/ncbi-blast/blastdb"
if [[ ! -d "${blastdb_path}" ]];then
    echo "creating blastdb directory: $blastdb_path"
    mkdir -p "${blastdb_path}"
else
    echo "blastdb directory: $blastdb_path"
fi

fasta_path="${BASE_PATH}/data/${PROJECT}/ncbi-blast/fasta"
if [[ ! -d "${fasta_path}" ]];then
    echo "creating fasta directory: $blastdb_path"
    mkdir -p "${fasta_path}"
else
    echo "fasta directory: $blastdb_path"
fi

./docker-net.sh || exit 1
docker compose -p $PROJECT up -d
