#!/bin/bash

# this experiment should demonstrate the power of the proagators
# run the coplete search with inorder branching on some small instances

SEED=0
TIMEOUT=300000
THREADS=4
CONFIGS='fff tff ftf ttf fft ttt'

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp1-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp1/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

# Run all the configurations on all instances
function runComplete {
  qProp=$(echo $5 | head -c +1)
  qrProp=$(echo $5 | head -c +2 | tail -c -1)
  xProp=$(echo $5 | head -c +3 | tail -c -1)
  if [ ! -f ${1}/$5-$(basename $2) ]; then
    ./bin/completeSearch $2 $3 $4 $qProp $qrProp $xProp ${1}/$5-$(basename $2)
  fi
}

export -f runComplete

parallel -j ${THREADS} runComplete ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: o ::: $CONFIGS

rm -rf $dir

printf "" > ${outdir}/stats-in
for i in ${instances}; do
  printf "$(basename $i)\n" >> ${outdir}/stats-in
  printf "${outdir}/fff-$(basename $i)\n" >> ${outdir}/stats-in
  printf "upperBoundQ\n${outdir}/tff-$(basename $i)\n" >> ${outdir}/stats-in
  printf "upperBoundQR\n${outdir}/ftf-$(basename $i)\n" >> ${outdir}/stats-in
  printf "upperBoundQQR\n${outdir}/ttf-$(basename $i)\n" >> ${outdir}/stats-in
  printf "upperBoundX\n${outdir}/fft-$(basename $i)\n" >> ${outdir}/stats-in
  printf "all\n${outdir}/ttt-$(basename $i)\n" >> ${outdir}/stats-in
  printf "\n" >> ${outdir}/stats-in
done
