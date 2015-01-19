#!/bin/bash
if [ "${useFILE1:+1}"="1" ]
then
    unset useFILE1
fi
NuWaConfigScript="/afs/ihep.ac.cn/users/l/lidj/.laf_opt"


#inputFILELISTPATH="/publicfs/dyb/data/rec/P12A/GoodRunList_v3/physics"
#outPUTFILEPATH="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12A"
#logPath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12A/log"
#targetdir=$PWD/P12A

##inputFILELISTPATH="/dybfs/rec/P12E/GoodRun_v4/minirec_peakCharge"
##inputFILELISTPATH="/dybfs/rec/P12E/GoodRun_v4/rec"
#inputFILELISTPATH="/dybfs/rec/P12E/GoodRun_v4/minirec"
#outPUTFILEPATH="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12E"
#logPath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12E/log"
#targetdir=$PWD/P12E

#inputFILELISTPATH="/dybfs/rec/P12B/GoodRun/minirec"
#RpcGoodFileList="/workfs/dyw/lidj/runlist/P12B/RpcGood"
#outPUTFILEPATH="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12B"
#logPath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12B/log"
#targetdir=$PWD/P12B

#inputFILELISTPATH="/dybfs/rec/P12C/GoodRun-part2-v0"
#RpcGoodFileList="/workfs/dyw/lidj/runlist/P12C/RpcGood/GoodRun-part2-v0"
#inputFILELISTPATH="/dybfs/rec/P12C/GoodRun"
##RpcGoodFileList="/workfs/dyw/lidj/runlist/P12C/RpcGood/GoodRun"
#outPUTFILEPATH="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12C"
#logPath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P12C/log"
#targetdir=$PWD/P12C


#inputFILELISTPATH="/dybfs/rec/P13A/GoodRun_v1_minirec/"
##inputFILELISTPATH="/dybfs/rec/P13A/GoodRun_v1"
##RpcGoodFileList="/workfs/dyw/lidj/runlist/P12B/RpcGood"
#outPUTFILEPATH="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P13A"
#logPath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P13A/log"
#targetdir=$PWD/P13A

inputFILELISTPATH="/dybfs/rec/P14A/GoodRun_v2"
#RpcGoodFileList="/workfs/dyw/lidj/runlist/P12B/RpcGood"
outPUTFILEPATH="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P14A"
logPath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P14A/log"
targetdir=$PWD/jobs/P14A

#inputFILELISTPATH="/junofs/dyb/P14B/minirec_GoodRun_v1/"
##RpcGoodFileList="/workfs/dyw/lidj/runlist/P12B/RpcGood"
#outPUTFILEPATH="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P14B"
#logPath="/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/P14B/log"
#targetdir=$PWD/jobs/P14B

outputFile2Name="runRUNNUM_IsotopesAna.root"
useFILE1="TOutputSvc.Output" #if not use FILE1 ,just comment out this line
outputFile1Name="runRUNNUM_hists.root"
inputSvc="ReconInputSvc" # MiniReconInputSvc or ReconInputSvc
useRPC="FALSE" #FALSE or TRUE
gendir=$PWD
sitePostfix=""

for site in EH1 EH2 EH3
do
    siteFullName=$site$sitePostfix
    if [ ! -d "$inputFILELISTPATH/$siteFullName" ]
    then
        echo "input file path doesn't exits, please check your config..."
        continue 
    fi
    if [ ! -d "$targetdir/$siteFullName" ]
    then
        mkdir -p "$targetdir/$siteFullName/"
        echo "mkdir scriptDir : $targetdir/$siteFullName/"
    fi
    if [ ! -d "$outPUTFILEPATH/$siteFullName" ]
    then
        mkdir -p "$outPUTFILEPATH/$siteFullName/"
        echo "mkdir dataDir : $outPUTFILEPATH/$siteFullName/"
    fi
    if [ ! -d "$logPath/$siteFullName" ]
    then
        mkdir -p "$logPath/$siteFullName/"
        echo "mkdir logDir : $logPath/$siteFullName/"
    fi
    
	firstlist=1
	pushd $inputFILELISTPATH/$site/
	for filename in `ls *.list `;
	do
		#  runNum=${filename:3:5}
		temprunNum1=${filename:3:5}
		if test "${temprunNum1//[0-9]/}" #is not int
		then
			temprunNum2=${filename:0:5}
			if test "${temprunNum2//[0-9]/}" #is not int
			then
				echo -e "\e[31;44;1m ! bad run number : ${filename}==>${temprunNum1} or ${temprunNum2} ,skip this list !\e[0m"
				continue
			else
				runNum=${filename:0:5}
			fi
		else
			runNum=${filename:3:5}
		fi

		echo $filename, $runNum
			sed -e "s/RUNNUM/${runNum}/g"  \
				-e "s#INPUTFILELISTPATH#${inputFILELISTPATH}#g"  \
			    -e "s#RPCGOODFILELIST#${RpcGoodFileList}#g"  \
				-e "s#FILENAME#${filename}#g"  \
				-e "s#RPCfILENAME#${runNum}.list#g"  \
				-e "s#INPUTSVC#${inputSvc}#g"  \
				-e "s#^//TOutputSvc.Output#${useFILE1:=//TOutputSvc.Output}#g"  \
				-e "s#FILE1NAME#${outputFile1Name/RUNNUM/$runNum}#g"  \
				-e "s#FILE2NAME#${outputFile2Name/RUNNUM/$runNum}#g"  \
				-e "s#USERPC#${useRPC}#g"  \
				-e "s#OUTPUTFILEPATH#${outPUTFILEPATH}#g"  \
				-e "s/SITE/$site/g"  $gendir/config_temp.txt > $targetdir/$site/config_run${runNum}.txt

			sed -e "s/RUNNUM/${runNum}/g" \
				-e "s#LOGPATH#$logPath#g"  \
				-e "s#TARGETDIR#$targetdir#g"  \
				-e "s#NuWaConfigScript#${NuWaConfigScript}#g"  \
				-e "s/SITE/$site/g"   $gendir/job_temp.csh > $targetdir/$site/job_${runNum}.csh
		if [ $firstlist -eq 1 ];then
			if [ -f "$targetdir/$site/submit.sh" ];then
				echo "submit.sh already exists,delete it,create a new one"
				rm -rf "$targetdir/$site/submit.sh"
			else
				echo "submit.sh not exists,create a new one"
			fi
			if [ -f "$targetdir/$site/submit.sh.shortq" ];then
				echo "submit.sh.shortq already exists,delete it,create a new one"
				rm -rf "$targetdir/$site/submit.sh.shortq"
			else
				echo "submit.sh.shortq not exists,create a new one"
			fi
		fi
		echo "qsub -q dyb64q job_${runNum}.csh" >> $targetdir/$site/submit.sh
		echo "qsub -q dybshortq job_${runNum}.csh" >> $targetdir/$site/submit.sh.shortq
		let "firstlist += 1"
	done
	popd
done

