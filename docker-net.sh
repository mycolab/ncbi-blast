#!/bin/bash

if [[ ! $(docker network ls -q -f name="${PROJECT}") ]]; then
  docker network create "${PROJECT}" 2> /dev/null
fi
