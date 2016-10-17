instanceDir=$2
SEED=$1

mkdir -p ${instanceDir}

TELE=1
TARG=5
HORI=5
MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=2
MAX_CAD=4
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
HORI=5
MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=2
MAX_CAD=4
MIN_P=1
MAX_P=1
VIS_P=0
OFFSET=0
BALANCE_N=4
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=2
TARG=10
HORI=50
MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=2
MAX_CAD=4
MIN_P=8
MAX_P=12
VIS_P=0
OFFSET=0
BALANCE_N=4
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=2
TARG=15
HORI=70
MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=2
MAX_CAD=4
MIN_P=8
MAX_P=12
VIS_P=0
OFFSET=0
BALANCE_N=4
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=3
TARG=15
HORI=5
MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=2
MAX_CAD=4
MIN_P=1
MAX_P=1
VIS_P=0
OFFSET=0
BALANCE_N=4
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=3
TARG=15
HORI=7
MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=2
MAX_CAD=4
MIN_P=1
MAX_P=1
VIS_P=0
OFFSET=0
BALANCE_N=4
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=2
TARG=20
HORI=200
MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=25
MAX_CAD=60
MIN_P=8
MAX_P=12
VIS_P=10
OFFSET=20
BALANCE_N=3
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=3
TARG=30
HORI=200
MIN_GAIN=5
MAX_GAIN=100
MIN_CAD=20
MAX_CAD=60
MIN_P=8
MAX_P=12
VIS_P=10
OFFSET=20
BALANCE_N=1
BALANCE_D=2

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=3
TARG=50
HORI=200
MIN_GAIN=15
MAX_GAIN=200
MIN_CAD=24
MAX_CAD=60
MIN_P=8
MAX_P=12
VIS_P=10
OFFSET=20
BALANCE_N=1
BALANCE_D=2

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=4
TARG=50
HORI=200
MIN_GAIN=15
MAX_GAIN=200
MIN_CAD=24
MAX_CAD=60
MIN_P=8
MAX_P=12
VIS_P=10
OFFSET=20
BALANCE_N=4
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

TELE=4
TARG=50
HORI=400
MIN_GAIN=15
MAX_GAIN=200
MIN_CAD=24
MAX_CAD=60
MIN_P=8
MAX_P=12
VIS_P=10
OFFSET=20
BALANCE_N=3
BALANCE_D=5

OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_P}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_P $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
instances=${instances}' '${OUTPUT}

echo ${instances}
