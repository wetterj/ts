#!/bin/bash

# this experiment should demonstrate the power of the proagators
# run the coplete search with inorder branching on some small instances

SEED=0
TIMEOUT=600000
THREADS=4
INIT_TOS='1000'
NH_TOS='500 1000 1500'
PHIS='30 50 100'
SEEDS=$(seq 5)

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp3-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp3/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

# Run all the configurations on all instances
function runGreedy {
  outFile=${1}/${4}-${5}-${6}-${7}-$(basename $2)
  if [ ! -f ${outFile} ]; then
    ./bin/greedy $7 $2 $3 $6 $4 $5 ${outFile}
  fi
}

function runGreedyBT {
  outFile=${1}/bt-${4}-${5}-${6}-${7}-$(basename $2)
  if [ ! -f ${outFile} ]; then
    ./bin/greedyBT $7 $2 $3 $6 $4 $5 ${outFile}
  fi
}

export -f runGreedy
export -f runGreedyBT

parallel -j ${THREADS} runGreedy ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${INIT_TOS} ::: ${NH_TOS} ::: ${PHIS} ::: ${SEEDS}
parallel -j ${THREADS} runGreedyBT ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${INIT_TOS} ::: ${NH_TOS} ::: ${PHIS} ::: ${SEEDS}

rm -rf $dir

#printf "" > ${outdir}/stats-in
#for i in ${instances}; do
#  printf "$(basename $i)\n" >> ${outdir}/stats-in
#  printf "${outdir}/fff-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQ\n${outdir}/tff-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQR\n${outdir}/ftf-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQQR\n${outdir}/ttf-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundX\n${outdir}/fft-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "all\n${outdir}/ttt-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "\n" >> ${outdir}/stats-in
#done
