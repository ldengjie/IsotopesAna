#!/bin/bash
gendir=$PWD


for data in P14A
do
for site in 1 2 3
do
for iso in  B12 N12 
do
for exRate in 0.05 0.1 0.5 1. 1.5 2. 2.5 3. 3.5 4. 
do
for isBinned in 1 
do
for reBIn in 5 1
do
			sed -e "s/DATA/${data}/g" \
				-e "s#SITE#$site#g"  \
				-e "s#ISO#$iso#g"  \
				-e "s#EXRATE#${exRate}#g"  \
				-e "s#ISBINNED#$isBinned#g"  \
				-e "s/REBIN/$reBIn/g"   $gendir/multi_temp.csh > multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh
		echo "qsub -q dyb64q multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh" >>multi_submit.sh
done
done
done
done
done
done
for data in P14A
do
for site in 1 2 3
do
for iso in  B12 N12 
do
for exRate in 0.05 0.1 0.5 1. 1.5 2. 2.5 3. 3.5 4. 
do
for isBinned in  0
do
for reBIn in  1
do
			sed -e "s/DATA/${data}/g" \
				-e "s#SITE#$site#g"  \
				-e "s#ISO#$iso#g"  \
				-e "s#EXRATE#${exRate}#g"  \
				-e "s#ISBINNED#$isBinned#g"  \
				-e "s/REBIN/$reBIn/g"   $gendir/multi_temp.csh > multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh
		echo "qsub -q dyb64q multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh" >>multi_submit.sh
done
done
done
done
done
done

for data in P14A
do
for site in 1 2 3
do
for iso in  Li8 C9
do
for exRate in 0.05 0.1 0.5 1. 1.5 2. 2.5 3. 3.5 4. 
do
for isBinned in 1 
do
for reBIn in 50 100 200
do
			sed -e "s/DATA/${data}/g" \
				-e "s#SITE#$site#g"  \
				-e "s#ISO#$iso#g"  \
				-e "s#EXRATE#${exRate}#g"  \
				-e "s#ISBINNED#$isBinned#g"  \
				-e "s/REBIN/$reBIn/g"   $gendir/multi_temp.csh > multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh
		echo "qsub -q dyb64q multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh" >> multi_submit.sh
done
done
done
done
done
done

for data in P14A
do
for site in 1 2 3
do
for iso in  Li8 C9
do
for exRate in 0.05 0.1 0.5 1. 1.5 2. 2.5 3. 3.5 4. 
do
for isBinned in  0
do
for reBIn in 50 
do
			sed -e "s/DATA/${data}/g" \
				-e "s#SITE#$site#g"  \
				-e "s#ISO#$iso#g"  \
				-e "s#EXRATE#${exRate}#g"  \
				-e "s#ISBINNED#$isBinned#g"  \
				-e "s/REBIN/$reBIn/g"   $gendir/multi_temp.csh > multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh
		echo "qsub -q dyb64q multiJob/${data}_EH${site}_${iso}_${exRate}_${isBinned}_${reBIn}.csh" >> multi_submit.sh
done
done
done
done
done
done
