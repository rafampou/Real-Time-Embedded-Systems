#!/bin/bash
echo '' > data_9186.txt
for i in $(seq 1 1 12)
do
  echo "p=64 q="$(( 2 ** $i));

  for j in $(seq 1 1 50)
  do
    ./prod-cons_9186 64 $(( 2 ** $i )) >> data_9186.txt;
  done
    echo $'' >> data_9186.txt;
  in=$((in+1));

done
