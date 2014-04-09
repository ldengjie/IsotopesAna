#/bin/tcsh
source ~/.laf_opt
cd /afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job/
chmod 777 GenIsoSpec.main 
setenv LD_LIBRARY_PATH /afs/ihep.ac.cn/users/l/lidj/file/IsotopesAna/job:$LD_LIBRARY_PATH
./GenIsoSpec.main P14A 2  >/afs/ihep.ac.cn/users/l/lidj/largedata/IsotopesAna/job/log.GenData_P14A_EH2
