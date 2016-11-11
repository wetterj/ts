#!/bin/bash

# this experiment should perform a parameter sweep of SA

SEED=0
TIMEOUT=600000
THREADS=4
ANT_TOS='500'
PHIS='100'
N_ANTS='4 8 16 32'
RHO='0.1 0.2 0.3'
SEEDS=$(seq 5)

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp5-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp5/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

instances="${instanceDir}/tele10.targ200.hori2000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset200.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele5.targ200.hori1000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset100.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele2.targ200.hori500.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset50.0.balanceN3.balanceD5"

# Run all the configurations on all instances
function runACO {
  outd=$1
  inst=$2
  to=$3
  ant_to=$4
  phi=$5
  n_ants=$6
  rho=$7
  seed=$8
  outFile=${outd}/${ant_to}-${phi}-${n_ants}-${rho}-${seed}-$(basename ${inst})
  if [ ! -f ${outFile} ]; then
    ./bin/aco ${seed} ${inst} ${to} ${ant_to} ${phi} ${n_ants} ${rho} ${outFile}
  fi
}

export -f runACO

parallel -j ${THREADS} runACO ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${ANT_TOS} ::: ${PHIS} ::: ${N_ANTS} ::: ${RHO} ::: ${SEEDS}

rm -rf $dir

printf "" > ${outdir}/stats-in
for i in ${instances}; do
  for nh in ${ANT_TOS}; do
    for phi in ${PHIS}; do
      for nAnts in ${N_ANTS}; do
        for rho in ${RHO}; do
          printf "${nAnts} ${rho} $(basename $i)" >> ${outdir}/stats-in
          for s in $SEEDS; do
            printf " ${outdir}/${nh}-${phi}-${nAnts}-${rho}-${s}-$(basename $i)" >> ${outdir}/stats-in
          done
          printf "\n" >> ${outdir}/stats-in
        done
      done
    done
  done
done
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
