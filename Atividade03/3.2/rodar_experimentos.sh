#!/bin/bash

PLATAFORMA=${1:-plataforma}
NUCLEOS=$2
REPETICOES=5
SAIDA="tempos_${PLATAFORMA}.csv"

gcc -O2 calcpi.c -o calcpi -lm -lpthread || exit 1

PREFIXO=""
[ -n "$NUCLEOS" ] && PREFIXO="taskset -c $NUCLEOS"

echo "== Máquina =="
lscpu | grep -E "Model name|^CPU\(s\)|Thread\(s\) per core|Core\(s\) per socket"
echo "Restrição de núcleos: ${NUCLEOS:-nenhuma}"
echo

echo "threads,execucao,tempo_real" > "$SAIDA"
TIMEFORMAT=%R

for T in 1 2 4 8 16 32; do
    for R in $(seq 1 $REPETICOES); do
        TEMPO=$( { time $PREFIXO ./calcpi $T > /dev/null; } 2>&1 )
        echo "$T,$R,$TEMPO" >> "$SAIDA"
        echo "threads=$T  execução=$R  tempo=${TEMPO}s"
    done
done

echo
echo "Resultados salvos em $SAIDA"
