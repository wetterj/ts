#!/bin/bash

# this experiment should demonstrate the different branching strategies

SEED=0
TIMEOUT=150000
THREADS=4
N_RANDOM=5
POWS='1 3 5 10 30 50 100'

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp1-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp2/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh ${SEED} ${instanceDir})

# Run all the configurations on all instances
function runComplete {
  # only one seed for non-random
  if [ $4 == r -o $4 == f -o $5 == 1 ]; then
    # all powers for fitness prop
    if [ $4 == f ]; then
      # only if there is no data
      if [ ! -f ${1}/$4-$6-$5-$(basename $2) ]; then
        ./bin/firstSolution $5 $2 $3 $4 $6 ${1}/$4-$6-$5-$(basename $2)
      fi
    # only one power for non-powered
    elif [ $6 == 1 ]; then
      # only if there is no data
      if [ ! -f ${1}/$4-$5-$(basename $2) ]; then
        ./bin/firstSolution $5 $2 $3 $4 ${1}/$4-$5-$(basename $2)
      fi
    fi
  fi
}

export -f runComplete

parallel -j ${THREADS} runComplete ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: b r f ::: $(seq ${N_RANDOM}) ::: $POWS

rm -rf $dir

printf "" > ${outdir}/stats-in
for i in ${instances}; do
  printf "$(basename $i)\n" >> ${outdir}/stats-in
  printf "${outdir}/b-1-$(basename $i)\n" >> ${outdir}/stats-in
  printf "random\n" >> ${outdir}/stats-in
  for s in $(seq ${N_RANDOM}); do
    printf "${outdir}/r-${s}-$(basename $i) " >> ${outdir}/stats-in
  done
  printf "\n" >> ${outdir}/stats-in
  for p in ${POWS}; do
    printf "fitness${p}\n" >> ${outdir}/stats-in
    for s in $(seq ${N_RANDOM}); do
        printf "${outdir}/f-${p}-${s}-$(basename $i) " >> ${outdir}/stats-in
    done
    printf "\n" >> ${outdir}/stats-in
  done
  printf "\n" >> ${outdir}/stats-in
done
