#!/bin/bash

# this experiment should demonstrate the different branching strategies

SEED=0
TIMEOUT=150000
THREADS=4
N_RANDOM=5
POWS='1 3 5'

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp1-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
outdir=experiments/exp2/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh $SEED instanceDir)

# Run all the configurations on all instances
function runComplete {
  # only one seed for non-random
  if [ $4 == r -o $5 == 1 ]; then
    # all powers for fitness prop
    if [ $4 == f ]; then
      # only if there is no data
      if [ ! -f ${1}/$4-$6-$5-$(basename $2) ]; then
        ./bin/completeSearch $2 $3 $4 $6 f f f ${1}/$4-$6-$5-$(basename $2)
      fi
    # only one power for non-powered
    elif [ $6 == 1 ]; then
      # only if there is no data
      if [ ! -f ${1}/$4-$5-$(basename $2) ]; then
        ./bin/completeSearch $2 $3 $4 f f f ${1}/$4-$5-$(basename $2)
      fi
    fi
  fi
}

export -f runComplete

parallel -j ${THREADS} runComplete ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: b r f ::: $(seq ${N_RANDOM}) ::: POWS

rm -rf $dir

## for each different branches print best solution
#printf "\tbestX\trandomMean\trandomStdDev\n" > ${outdir}/quals
#for i in ${instances}; do
#  d=$(head -n +3 ${outdir}/b-1-$(basename $i) | tail -n -1)
#  d=($d)
#  printf "\t${d[1]}\t" >> ${outdir}/quals
#  nums=''
#  for s in $(seq ${N_RANDOM});do
#    d=$(head -n +3 ${outdir}/r-${s}-$(basename $i) | tail -n -1)
#    d=($d)
#    nums="$nums ${d[1]}"
#  done
#  ave=$(python scripts/mean.py $nums)
#  std=$(python scripts/stddev.py $nums)
#  echo $nums
#  printf "${ave}\t${std}\n" >> ${outdir}/quals
#done
#
## for each config print timeout crossed
#printf '' > ${outdir}/timeout
#for c in $CONFIGS;do
#  printf "\t$c" >> ${outdir}/timeout
#done
#printf "\n" >> ${outdir}/timeout
#
#for i in ${instances}; do
#  for c in $CONFIGS;do
#    d=$(head -n +4 ${outdir}/${c}-$(basename $i) | tail -n -1)
#    d=($d)
#    printf "\t${d[1]}" >> ${outdir}/timeout
#  done
#  printf "\n" >> ${outdir}/timeout
#done
#
## for each config print time to best solution
#printf '' > ${outdir}/time
#for c in $CONFIGS;do
#  printf "\t$c" >> ${outdir}/time
#done
#printf "\n" >> ${outdir}/time
#
#for i in ${instances}; do
#  for c in $CONFIGS;do
#    d=$(head -n +1 ${outdir}/${c}-$(basename $i))
#    d=($d)
#    printf "\t${d[1]}" >> ${outdir}/time
#  done
#  printf "\n" >> ${outdir}/time
#done
#
## for each config print time total
#printf '' > ${outdir}/close-time
#for c in $CONFIGS;do
#  printf "\t$c" >> ${outdir}/close-time
#done
#printf "\n" >> ${outdir}/close-time
#
#for i in ${instances}; do
#  for c in $CONFIGS;do
#    d=$(head -n +1 ${outdir}/${c}-$(basename $i))
#    d=($d)
#    printf "\t${d[1]}" >> ${outdir}/close-time
#  done
#  printf "\n" >> ${outdir}/close-time
#done
#
## for each config print fails to best solution
#printf '' > ${outdir}/fails
#for c in $CONFIGS;do
#  printf "\t$c" >> ${outdir}/fails
#done
#printf "\n" >> ${outdir}/fails
#
#for i in ${instances}; do
#  for c in $CONFIGS;do
#    d=$(head -n +2 ${outdir}/${c}-$(basename $i) | tail -n -1)
#    d=($d)
#    printf "\t${d[1]}" >> ${outdir}/fails
#  done
#  printf "\n" >> ${outdir}/fails
#done
#
## for each config print fails total
#printf '' > ${outdir}/close-fails
#for c in $CONFIGS;do
#  printf "\t$c" >> ${outdir}/close-fails
#done
#printf "\n" >> ${outdir}/close-fails
#
#for i in ${instances}; do
#  for c in $CONFIGS;do
#    d=$(tail -n -1 ${outdir}/${c}-$(basename $i))
#    d=($d)
#    printf "\t${d[1]}" >> ${outdir}/close-fails
#  done
#  printf "\n" >> ${outdir}/close-fails
#done
#
