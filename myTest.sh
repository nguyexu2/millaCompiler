#!/bin/bash
cd samples
for i in *.mila ; do
  echo "-------------------------------------"
  j="${i%%.*}"
  ../mila "$i"
  echo "-------------------------------------"
done
