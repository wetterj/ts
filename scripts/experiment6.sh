#!/bin/bash

# this experiment should perform a parameter sweep of SA

SEED=0
TIMEOUT=600000
THREADS=4
ANT_TOS='500'
PHIS='100'
N_ANTS='32'
RHO='0.1'
N_LOCALS='2 3 4 5 6'
NH_TO=500
N_ITERS='5 7 9 11 13 15'
SEEDS=$(seq 5)

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp6-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp6/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

instances="${instanceDir}/tele10.targ200.hori2000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset200.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele5.targ200.hori1000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset100.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele2.targ200.hori500.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset50.0.balanceN3.balanceD5"

# Run all the configurations on all instances
function runACOGreedy {
  outd=$1
  inst=$2
  to=$3
  ant_to=$4
  phi=$5
  n_ants=$6
  rho=$7
  nLocals=$8
  nhTimeout=$9
  nhIters=${10}
  seed=${11}
  outFile=${outd}/${ant_to}-${phi}-${n_ants}-${rho}-${nLocals}-${nhIters}-${seed}-$(basename ${inst})
  if [ ! -f ${outFile} ]; then
    ./bin/aco-greedy ${seed} ${inst} ${to} ${ant_to} ${phi} ${n_ants} ${rho} ${nLocals} ${nhTimeout} ${nhIters} ${outFile}
  fi
}

export -f runACOGreedy

parallel -j ${THREADS} runACOGreedy ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${ANT_TOS} ::: ${PHIS} ::: ${N_ANTS} ::: ${RHO} ::: ${N_LOCALS} ::: ${NH_TO} ::: ${N_ITERS} ::: ${SEEDS}

rm -rf $dir

printf "" > ${outdir}/stats-in
for i in ${instances}; do
  for nh in ${ANT_TOS}; do
    for phi in ${PHIS}; do
      for nLocal in ${N_LOCALS}; do
        for iters in ${N_ITERS}; do
          printf "${nLocal} ${iters} $(basename $i)" >> ${outdir}/stats-in
          for s in $SEEDS; do
            printf " ${outdir}/${nh}-${phi}-${N_ANTS}-${RHO}-${nLocal}-${iters}-${s}-$(basename $i)" >> ${outdir}/stats-in
          done
          printf "\n" >> ${outdir}/stats-in
        done
      done
    done
  done
done
