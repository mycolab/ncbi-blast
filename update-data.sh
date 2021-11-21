#!/usr/bin/env bash
DATASETS=nt patnt taxdb 18S_fungal_sequences 28S_fungal_sequences ITS_RefSeq_Fungi

# shellcheck disable=SC2153
for DATASET in ${DATASETS}
do
    update_blastdb.pl "${DATASET}" --decompress
done