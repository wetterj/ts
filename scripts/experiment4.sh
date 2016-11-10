#!/bin/bash

# this experiment should perform a parameter sweep of SA

SEED=0
TIMEOUT=600000
THREADS=4
INIT_TOS='1000'
NH_TOS='500'
PHIS='100'
INIT_TEMP='0.5 0.75 1.0 1.25 1.5'
COOL_RATE='0.001 0.0031622776602 0.01 0.031622776602 0.1 0.31622776602'
SEEDS=$(seq 5)

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp4-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp4/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

instances="${instanceDir}/tele10.targ200.hori2000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset200.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele5.targ200.hori1000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset100.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele2.targ200.hori500.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset50.0.balanceN3.balanceD5"

# Run all the configurations on all instances
function runSA {
  outd=$1
  inst=$2
  to=$3
  init_to=$4
  nh_to=$5
  phi=$6
  init_tmp=$7
  cool_rate=$8
  seed=$9
  outFile=${outd}/${nh_to}-${phi}-${init_tmp}-${cool_rate}-${seed}-$(basename ${inst})
  if [ ! -f ${outFile} ]; then
    ./bin/simulatedAnnealing ${seed} ${inst} ${to} ${init_to} ${nh_to} ${phi} ${init_tmp} ${cool_rate} ${outFile}
  fi
}

export -f runSA

parallel -j ${THREADS} runSA ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${INIT_TOS} ::: ${NH_TOS} ::: ${PHIS} ::: ${INIT_TEMP} ::: ${COOL_RATE} ::: ${SEEDS}

rm -rf $dir

printf "" > ${outdir}/stats-in
for i in ${instances}; do
  for nh in ${NH_TOS}; do
    for phi in ${PHIS}; do
      for initTmp in ${INIT_TEMP}; do
        for cr in ${COOL_RATE}; do
          printf "${initTmp} ${cr} $(basename $i)" >> ${outdir}/stats-in
          for s in $SEEDS; do
            printf " ${outdir}/${nh}-${phi}-${initTmp}-${cr}-${s}-$(basename $i)" >> ${outdir}/stats-in
          done
          printf "\n" >> ${outdir}/stats-in
        done
      done
    done
  done
done
#  printf "${outdir}/fff-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQ\n${outdir}/tff-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQR\n${outdir}/ftf-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundQQR\n${outdir}/ttf-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "upperBoundX\n${outdir}/fft-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "all\n${outdir}/ttt-$(basename $i)\n" >> ${outdir}/stats-in
#  printf "\n" >> ${outdir}/stats-in
#done
