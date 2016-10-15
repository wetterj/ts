#!/bin/bash

# this experiment should demonstrate the power of the proagators
# run the coplete search with inorder branching on some small instances

SEED=0
TIMEOUT=120000
THREADS=4

# The directory to put the tmp stuff
dir=$(mktemp -d 'exp1-XXXXX')
instanceDir=${dir}/instances
mkdir ${instanceDir}
instances=''
outdir=experiments/exp1/
mkdir -p ${outdir}

# generate some instances
TELE=1
TARG=10
HORI=10
MIN_GAIN=1
MAX_GAIN=5
MIN_CAD=5
MAX_CAD=5
MIN_P=1
MAX_P=1
VIS_P=0
OFFSET=0
BALANCE_N=1
BALANCE_D=2

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=2
TARG=10
HORI=10
MIN_GAIN=1
MAX_GAIN=5
MIN_CAD=5
MAX_CAD=5
MIN_P=1
MAX_P=1
VIS_P=0
OFFSET=0
BALANCE_N=1
BALANCE_D=2

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=2
TARG=15
HORI=100
MIN_GAIN=1
MAX_GAIN=5
MIN_CAD=5
MAX_CAD=5
MIN_P=9
MAX_P=11
VIS_P=0
OFFSET=0
BALANCE_N=3
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=3
TARG=15
HORI=100
MIN_GAIN=1
MAX_GAIN=5
MIN_CAD=5
MAX_CAD=5
MIN_P=9
MAX_P=11
VIS_P=0
OFFSET=0
BALANCE_N=3
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

# Run all the configurations on all instances
function runComplete {
  ./bin/completeSearch $2 $3 $4 $5 $6 $7 | tail -n -3 > ${1}/$5$6$7$(basename $2)
}

export -f runComplete

parallel -j ${THREADS} runComplete ${outdir} ::: ${instances} ::: ${TIMEOUT} ::: o ::: f t ::: f t ::: f t

rm -rf $dir
