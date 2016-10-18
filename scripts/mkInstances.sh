instanceDir=$2
SEED=$1

mkdir -p ${instanceDir}

NTELES='2 5 10'
NTARGETS='50 100 200'
HORIZONS='50 100 200 500 1000 2000'
BNS='0 3'
BDS='5'

MIN_GAIN=10
MAX_GAIN=100
MIN_CAD=8
MAX_CAD=12
MIN_P=20
MAX_P=100
VIS_P=10

# a couple of small instances
for TELE in 1 2; do
  for TARG in 5 10; do
    for HORI in $HORIZONS; do
      for BALANCE_N in $BNS; do
        for BALANCE_D in $BDS; do
          if [ $TELE == 2 -o $BALANCE_N == 0 ]; then
            VIS_SMALL=0
            OFFSET=0
            OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_SMALL}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
            python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_SMALL $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
            instances=${instances}' '${OUTPUT}
          fi
        done
      done
    done
  done
done

## The rest
#for TELE in $NTELES; do
#  for TARG in $NTARGETS; do
#    for HORI in $HORIZONS; do
#      for BALANCE_N in $BNS; do
#        for BALANCE_D in $BDS; do
#          OFFSET=$(echo "$HORI * 0.1" | bc -l)
#          OUTPUT=${instanceDir}/tele${TELE}.targ${TARG}.hori${HORI}.minGain${MIN_GAIN}.maxGain${MAX_GAIN}.minCadence${MIN_CAD}.maxCadence${MAX_CAD}.minPeriod${MIN_P}.maxPeriod${MAX_P}.vis${VIS_SMALL}.offset${OFFSET}.balanceN${BALANCE_N}.balanceD${BALANCE_D}
#          python telescopes/python/mkInstance.py $SEED $TELE $TARG $HORI $MIN_GAIN $MAX_GAIN $MIN_CAD $MAX_CAD $MIN_P $MAX_P $VIS_SMALL $OFFSET $BALANCE_N $BALANCE_D $OUTPUT
#          instances=${instances}' '${OUTPUT}
#        done
#      done
#    done
#  done
#done

echo ${instances}
