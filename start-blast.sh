#!/usr/bin/env bash

function usage(){
    echo "start-blast.sh [opts]"
    echo
    echo "options:"
    echo "  -h | --help     Show this help message"
    echo "  -u | --update   Update blast databases"
    echo "  -a | --api      Start the blast REST API"
    echo
}

OPTS=$(getopt \
    -o husU:P: \
    -l update,api,user:,pass: \
    -n 'start-blast.sh' -- "$@")

# shellcheck disable=SC2181
if [[ $? != 0 ]];then
    usage >&2
    exit 1
fi

eval set -- "${OPTS}"
HELP=false
UPDATE=false
API=false
while true; do
    case "$1" in
        -h|--help)        export HELP=true; usage; shift 1;;
        -u|--update)      export UPDATE=true; shift 1;;
        -a|--api)         export API=true; shift 1;;
        -U|--user)        export username=$2; shift 2;;
        -P|--pass)        export password=$2; shift 2;;
        --) shift 1; break;;
        *) usage; break;;
    esac
done

# run update in background
if [[ $UPDATE == true ]];then
    update-data.sh &
fi

# start the REST API
if [[ $API == true ]];then
    # TODO: locate and build CGI, or create custom API from CLI tools :(
    while true
    do
        sleep 1
    done
fi
