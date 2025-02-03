#!/bin/bash

ls /unity

## Create target directories for copy files
mkdir -p /engine/libs/unity/ || true
mkdir -p /engine/scripts/unity/ || true

## Copy the Unity test framework files
cp -r /unity/src/* /engine/libs/unity/ || true
cp -r /unity/auto/* /engine/scripts/unity/ || true

## Keep the container running
tail -f /dev/null
