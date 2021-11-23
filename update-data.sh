#!/usr/bin/env bash

WORKDIR=/blast/blastdb
DATASETS="18S_fungal_sequences 28S_fungal_sequences ITS_RefSeq_Fungi taxdb patnt nt"

cd $WORKDIR || exit 1

# shellcheck disable=SC2153
for DATASET in ${DATASETS}
do
    echo "updating database: ${DATASET}"
    update_blastdb.pl "${DATASET}" --decompress
done