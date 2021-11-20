#!/usr/bin/env bash
export PROJECT=mycolab
./docker-net.sh
docker compose -p $PROJECT up -d
