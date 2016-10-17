#!/bin/bash

# this experiment should demonstrate the different branching strategies

SEED=0
TIMEOUT=150000
THREADS=4
N_RANDOM=5

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
  if [ $4 == r -o $5 == 1 ]; then
    if [ ! -f ${1}/$4-$5-$(basename $2) ]; then
      ./bin/completeSearch $2 $3 $4 f f f | tail -n -6 > ${1}/$4-$5-$(basename $2)
    fi
  fi
}

export -f runComplete

parallel -j ${THREADS} runComplete ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: b r ::: $(seq ${N_RANDOM})

rm -rf $dir

# for each different branches print best solution
printf "\tbestX\trandomMean\trandomStdDev\n" > ${outdir}/quals
for i in ${instances}; do
  d=$(head -n +3 ${outdir}/b-1-$(basename $i) | tail -n -1)
  d=($d)
  printf "\t${d[1]}\t" >> ${outdir}/quals
  nums=''
  for s in $(seq ${N_RANDOM});do
    d=$(head -n +3 ${outdir}/r-${s}-$(basename $i) | tail -n -1)
    d=($d)
    nums="$nums ${d[1]}"
  done
  ave=$(python scripts/mean.py $nums)
  std=$(python scripts/stddev.py $nums)
  printf "${ave}\t${std}\n" >> ${outdir}/quals
done

# for each different branches print timeout
printf "\tbestX\trandomMean\trandomStdDev\n" > ${outdir}/timeout
for i in ${instances}; do
  d=$(head -n +4 ${outdir}/b-1-$(basename $i) | tail -n -1)
  d=($d)
  printf "\t${d[1]}\t" >> ${outdir}/timeout
  nums=''
  for s in $(seq ${N_RANDOM});do
    d=$(head -n +4 ${outdir}/r-${s}-$(basename $i) | tail -n -1)
    d=($d)
    nums="$nums ${d[1]}"
  done
  ave=$(python scripts/mean.py $nums)
  std=$(python scripts/stddev.py $nums)
  printf "${ave}\t${std}\n" >> ${outdir}/timeout
done

# for each different branches print time
printf "\tbestX\trandomMean\trandomStdDev\n" > ${outdir}/time
for i in ${instances}; do
  d=$(head -n +1 ${outdir}/b-1-$(basename $i) | tail -n -1)
  d=($d)
  printf "\t${d[1]}\t" >> ${outdir}/time
  nums=''
  for s in $(seq ${N_RANDOM});do
    d=$(head -n +1 ${outdir}/r-${s}-$(basename $i) | tail -n -1)
    d=($d)
    nums="$nums ${d[1]}"
  done
  ave=$(python scripts/mean.py $nums)
  std=$(python scripts/stddev.py $nums)
  printf "${ave}\t${std}\n" >> ${outdir}/time
done

# for each config print fails to best solution
printf "\tbestX\trandomMean\trandomStdDev\n" > ${outdir}/fails
for i in ${instances}; do
  d=$(head -n +2 ${outdir}/b-1-$(basename $i) | tail -n -1)
  d=($d)
  printf "\t${d[1]}\t" >> ${outdir}/fails
  nums=''
  for s in $(seq ${N_RANDOM});do
    d=$(head -n +2 ${outdir}/r-${s}-$(basename $i) | tail -n -1)
    d=($d)
    nums="$nums ${d[1]}"
  done
  ave=$(python scripts/mean.py $nums)
  std=$(python scripts/stddev.py $nums)
  printf "${ave}\t${std}\n" >> ${outdir}/fails
done
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
