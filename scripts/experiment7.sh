#!/bin/bash

# this experiment should use all solvers on all instances

SEED=0
TIMEOUT=600000
THREADS=4
SEEDS=$(seq 5)

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp7-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp7/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

instances="${instanceDir}/tele10.targ200.hori2000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset200.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele5.targ200.hori1000.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset100.0.balanceN3.balanceD5"
instances="${instances} ${instanceDir}/tele2.targ200.hori500.minGain10.maxGain100.minCadence20.maxCadence100.minPeriod8.maxPeriod12.vis10.offset50.0.balanceN3.balanceD5"

# Run all the configurations on all instances
function runAll {
  outd=$1
  inst=$2
  to=$3
  seed=$4

  # complete search
  outFile=${outdir}/complete-$(basename ${inst})
  if [ ! -f ${outFile} ]; then
    ./bin/completeSearch ${inst} ${to} b t t t ${outFile}
  fi

  # greedy
  outfile=${outdir}/greedy-${seed}-$(basename ${inst})
  if [ ! -f ${outFile} ]; then
    ./bin/greedy ${seed} ${inst} ${to} 100 1000 500 ${outFile}
  fi

  # simulated annealing
  outfile=${outdir}/sa-${seed}-$(basename ${inst})
  if [ ! -f ${outFile} ]; then
    ./bin/simulatedAnnealing ${seed} ${inst} 1000 500 100 1.0 0.001 ${outFile}
  fi

  # aco-greedy
}

export -f runACO

parallel -j ${THREADS} runACO ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: ${ANT_TOS} ::: ${PHIS} ::: ${N_ANTS} ::: ${RHO} ::: ${N_LOCALS} ::: ${NH_TO} ::: ${N_ITERS} ::: ${SEEDS}

rm -rf $dir

