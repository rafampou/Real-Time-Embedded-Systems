#!/bin/bash
echo '' > data64_9186.txt
for i in $(seq 1 1 12)
do
  echo "p=64 q="$(( 2 ** $i));

  for j in $(seq 1 1 200)
  do
    ./prod-cons_9186 64 $(( 2 ** $i )) >> data64_9186.txt;
  done
    echo $'' >> data64_9186.txt;
  in=$((in+1));

done
echo '' > data32_9186.txt
for i in $(seq 1 1 12)
do
  echo "p=32 q="$(( 2 ** $i));

  for j in $(seq 1 1 200)
  do
    ./prod-cons_9186 32 $(( 2 ** $i )) >> data32_9186.txt;
  done
    echo $'' >> data32_9186.txt;
  in=$((in+1));

done

echo '' > data4_9186.txt
for i in $(seq 1 1 12)
do
  echo "p=4 q="$(( 2 ** $i));

  for j in $(seq 1 1 200)
  do
    ./prod-cons_9186 4 $(( 2 ** $i )) >> data4_9186.txt;
  done
    echo $'' >> data4_9186.txt;
  in=$((in+1));

done
