#!/bin/bash

# this experiment should demonstrate the power of the proagators
# run the coplete search with inorder branching on some small instances

SEED=0
TIMEOUT=600000
THREADS=4
INIT_TOS='1000'
NH_TOS='500 1000 1500 2000'
PHIS='30 50 75 100 125 150'
SEEDS=$(seq 5)

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp3-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp3/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

instances="${instanceDir}/tele10.targ200.hori2000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset200.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele5.targ200.hori1000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset100.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele2.targ200.hori500.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset50.0.balanceN3.balanceD5"

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
#    echo "./bin/greedyBT $7 $2 $3 $6 $4 $5 ${outFile}"
    ./bin/greedyBT $7 $2 $3 $6 $4 $5 ${outFile}
  fi
}

export -f runGreedy
export -f runGreedyBT

parallel -j ${THREADS} runGreedy ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${INIT_TOS} ::: ${NH_TOS} ::: ${PHIS} ::: ${SEEDS}
parallel -j ${THREADS} runGreedyBT ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${INIT_TOS} ::: ${NH_TOS} ::: ${PHIS} ::: ${SEEDS}

rm -rf $dir

printf "" > ${outdir}/stats-in
printf "greedy\n" >> ${outdir}/stats-in
for i in ${instances}; do
  for init in ${INIT_TOS}; do
    for nh in ${NH_TOS}; do
      for phi in ${PHIS}; do
        printf "${init} ${nh} ${phi} $(basename $i)" >> ${outdir}/stats-in
        for s in $SEEDS; do
          printf " ${outdir}/${init}-${nh}-${phi}-${s}-$(basename $i)" >> ${outdir}/stats-in
        done
        printf "\n" >> ${outdir}/stats-in
      done
    done
  done
done
printf "greedyBT\n" >> ${outdir}/stats-in
for i in ${instances}; do
  for init in ${INIT_TOS}; do
    for nh in ${NH_TOS}; do
      for phi in ${PHIS}; do
        printf "${init} ${nh} ${phi} $(basename $i)" >> ${outdir}/stats-in
        for s in $SEEDS; do
          printf " ${outdir}/bt-${init}-${nh}-${phi}-${s}-$(basename $i)" >> ${outdir}/stats-in
        done
        printf "\n" >> ${outdir}/stats-in
      done
    done
  done
done
#  printf "$(basename $i)\n" >> ${outdir}/stats-in
#  printf "${outdir}/fff-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQ\n${outdir}/tff-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQR\n${outdir}/ftf-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQQR\n${outdir}/ttf-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundX\n${outdir}/fft-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "all\n${outdir}/ttt-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "\n" >> ${outdir}/stats-in
#done
