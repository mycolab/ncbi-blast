#!/bin/bash
if [ -f venv/bin/activate ]; then 
  source venv/bin/activate
fi

if [[ ! $(docker network ls -q -f name="${PROJECT}") ]]; then
  docker network create "${PROJECT}" 2> /dev/null
fi
