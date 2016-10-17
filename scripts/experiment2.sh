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
outdir=experiments/exp1/
mkdir -p ${outdir}

# generate some instances
instances=$(bash scripts/mkInstances.sh $SEED instanceDir)

# Run all the configurations on all instances
function runComplete {
  if [ $4 == r -o $5 == 0 ]; then
    if [ ! -f ${1}/$5-$(basename $2) ]; then
      ./bin/completeSearch $2 $3 $4 f f f | tail -n -6 > ${1}/$4-$5-$(basename $2)
    fi
  fi
}

export -f runComplete

parallel -j ${THREADS} runComplete ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: o r ::: $(range ${N_RANDOM})

rm -rf $dir

# for each config print best solution
printf '' > ${outdir}/quals
for c in $CONFIGS;do
  printf "\t$c" >> ${outdir}/quals
done
printf "\n" >> ${outdir}/quals

for i in ${instances}; do
  for c in $CONFIGS;do
    d=$(head -n +3 ${outdir}/${c}-$(basename $i) | tail -n -1)
    d=($d)
    printf "\t${d[1]}" >> ${outdir}/quals
  done
  printf "\n" >> ${outdir}/quals
done

# for each config print timeout crossed
printf '' > ${outdir}/timeout
for c in $CONFIGS;do
  printf "\t$c" >> ${outdir}/timeout
done
printf "\n" >> ${outdir}/timeout

for i in ${instances}; do
  for c in $CONFIGS;do
    d=$(head -n +4 ${outdir}/${c}-$(basename $i) | tail -n -1)
    d=($d)
    printf "\t${d[1]}" >> ${outdir}/timeout
  done
  printf "\n" >> ${outdir}/timeout
done

# for each config print time to best solution
printf '' > ${outdir}/time
for c in $CONFIGS;do
  printf "\t$c" >> ${outdir}/time
done
printf "\n" >> ${outdir}/time

for i in ${instances}; do
  for c in $CONFIGS;do
    d=$(head -n +1 ${outdir}/${c}-$(basename $i))
    d=($d)
    printf "\t${d[1]}" >> ${outdir}/time
  done
  printf "\n" >> ${outdir}/time
done

# for each config print time total
printf '' > ${outdir}/close-time
for c in $CONFIGS;do
  printf "\t$c" >> ${outdir}/close-time
done
printf "\n" >> ${outdir}/close-time

for i in ${instances}; do
  for c in $CONFIGS;do
    d=$(head -n +1 ${outdir}/${c}-$(basename $i))
    d=($d)
    printf "\t${d[1]}" >> ${outdir}/close-time
  done
  printf "\n" >> ${outdir}/close-time
done

# for each config print fails to best solution
printf '' > ${outdir}/fails
for c in $CONFIGS;do
  printf "\t$c" >> ${outdir}/fails
done
printf "\n" >> ${outdir}/fails

for i in ${instances}; do
  for c in $CONFIGS;do
    d=$(head -n +2 ${outdir}/${c}-$(basename $i) | tail -n -1)
    d=($d)
    printf "\t${d[1]}" >> ${outdir}/fails
  done
  printf "\n" >> ${outdir}/fails
done

# for each config print fails total
printf '' > ${outdir}/close-fails
for c in $CONFIGS;do
  printf "\t$c" >> ${outdir}/close-fails
done
printf "\n" >> ${outdir}/close-fails

for i in ${instances}; do
  for c in $CONFIGS;do
    d=$(tail -n -1 ${outdir}/${c}-$(basename $i))
    d=($d)
    printf "\t${d[1]}" >> ${outdir}/close-fails
  done
  printf "\n" >> ${outdir}/close-fails
done

#printf "\tfff\tttf\tfft\tttt\n" > ${outdir}/quals
#for i in ${instances}; do
#  fff=$(head -n +3 ${outdir}/fff-$(basename $i) | tail -n -1)
#  fff=($fff)
#  ttf=$(head -n +3 ${outdir}/ttf-$(basename $i) | tail -n -1)
#  ttf=($ttf)
#  fft=$(head -n +3 ${outdir}/fft-$(basename $i) | tail -n -1)
#  fft=($fft)
#  ttt=$(head -n +3 ${outdir}/ttt-$(basename $i) | tail -n -1)
#  ttt=($ttt)
#  printf "\t${fff[1]}\t${ttf[1]}\t${fft[1]}\t${ttt[1]}\n" >> ${outdir}/quals
#done
#
## the search time to best sol
#printf "\tfff\tttf\tfft\tttt\n" > ${outdir}/time
#for i in ${instances}; do
#  fff=$(head -n +1 ${outdir}/fff-$(basename $i))
#  fff=($fff)
#  ttf=$(head -n +1 ${outdir}/ttf-$(basename $i))
#  ttf=($ttf)
#  fft=$(head -n +1 ${outdir}/fft-$(basename $i))
#  fft=($fft)
#  ttt=$(head -n +1 ${outdir}/ttt-$(basename $i))
#  ttt=($ttt)
#  printf "\t${fff[1]}\t${ttf[1]}\t${fft[1]}\t${ttt[1]}\n" >> ${outdir}/time
#done
#
## the failures to best sol
#printf "\tfff\tttf\tfft\tttt\n" > ${outdir}/fails
#for i in ${instances}; do
#  fff=$(head -n +2 ${outdir}/fff-$(basename $i) | tail -n -1)
#  fff=($fff)
#  ttf=$(head -n +2 ${outdir}/ttf-$(basename $i) | tail -n -1)
#  ttf=($ttf)
#  fft=$(head -n +2 ${outdir}/fft-$(basename $i) | tail -n -1)
#  fft=($fft)
#  ttt=$(head -n +2 ${outdir}/ttt-$(basename $i) | tail -n -1)
#  ttt=($ttt)
#  printf "\t${fff[1]}\t${ttf[1]}\t${fft[1]}\t${ttt[1]}\n" >> ${outdir}/fails
#done
#
## is each one closed or time out
#printf "\tfff\tttf\tfft\tttt\n" > ${outdir}/timeout
#for i in ${instances}; do
#  fff=$(head -n +4 ${outdir}/fff-$(basename $i) | tail -n -1)
#  fff=($fff)
#  ttf=$(head -n +4 ${outdir}/ttf-$(basename $i) | tail -n -1)
#  ttf=($ttf)
#  fft=$(head -n +4 ${outdir}/fft-$(basename $i) | tail -n -1)
#  fft=($fft)
#  ttt=$(head -n +4 ${outdir}/ttt-$(basename $i) | tail -n -1)
#  ttt=($ttt)
#  printf "\t${fff[1]}\t${ttf[1]}\t${fft[1]}\t${ttt[1]}\n" >> ${outdir}/timeout
#done
#
## time to close it
#printf "\tfff\tttf\tfft\tttt\n" > ${outdir}/close-time
#for i in ${instances}; do
#  fff=$(head -n +5 ${outdir}/fff-$(basename $i) | tail -n -1)
#  fff=($fff)
#  ttf=$(head -n +5 ${outdir}/ttf-$(basename $i) | tail -n -1)
#  ttf=($ttf)
#  fft=$(head -n +5 ${outdir}/fft-$(basename $i) | tail -n -1)
#  fft=($fft)
#  ttt=$(head -n +5 ${outdir}/ttt-$(basename $i) | tail -n -1)
#  ttt=($ttt)
#  printf "\t${fff[1]}\t${ttf[1]}\t${fft[1]}\t${ttt[1]}\n" >> ${outdir}/close-time
#done
#
## fails to close it
#printf "\tfff\tttf\tfft\tttt\n" > ${outdir}/close-fails
#for i in ${instances}; do
#  fff=$(tail -n -1 ${outdir}/fff-$(basename $i) )
#  fff=($fff)
#  ttf=$(tail -n -1 ${outdir}/ttf-$(basename $i) )
#  ttf=($ttf)
#  fft=$(tail -n -1 ${outdir}/fft-$(basename $i) )
#  fft=($fft)
#  ttt=$(tail -n -1 ${outdir}/ttt-$(basename $i) )
#  ttt=($ttt)
#  printf "\t${fff[1]}\t${ttf[1]}\t${fft[1]}\t${ttt[1]}\n" >> ${outdir}/close-fails
#done