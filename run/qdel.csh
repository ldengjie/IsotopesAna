#!
set argnum=$#
echo $argvnum
@ i=1
set num=''
while($i <= $argvnum)
    set num="${num}0 "
    @ i++
end
set iargv=($num)
@ i=1
while($i <= $argvnum)
    if
end


exit
set username='zhaoj'
#set username=$user
@ jobnum=`qstat | grep $username -c`
if ( $jobnum == 0 ) then
    echo "  no job !"
    exit
endif
#set queuename="dyb64q"
set queuename='public'
@ jobnum=`qstat | grep $username | grep $queuename -c`
if ( $jobnum == 0 ) then
    echo "  you don't have jobs on server : $queuename !"
    exit
endif


#exit

@ i=1
set num=''
while($i <= $jobnum)
    set num="${num}0 "
    @ i++
end
set JobId=($num)
set queue=($num)

@ i=1
foreach jobid(`qstat | $grepstr | awk 'BEGIN{FS="."}{print $1}'`)
    set JobId[$i]=$jobid
    @ i++
end

#@ i=1
#foreach id($JobId)
#    set queue[$i]=`qstat -f -1 $id | grep "queue" | awk 'BEGIN{FS="="}{print $2}'`
#    @ i++
#end
#@ i=1
#set queuenameIsTrue=0
#while($i <= $jobnum)
#    echo "$queue[$i]"
#    if( $queue[$i] == $queuename ) then
#        set queuenameIsTrue=1
#        break
#    endif
#    @ i++
#end
#if( $queuenameIsTrue == 0 ) then
#    echo "  you don't have jobs on $queuename !"
#    exit
#endif

set submit_args=($num)
set init_work_dir=($num)
set job_state=($num)
@ i=1
foreach id($JobId)
#    echo $id
    set job_state[$i]=`qstat -f -1 $id | grep "job_state" | awk 'BEGIN{FS="="}{print $2}'`
    set submit_args[$i]=`qstat -f -1 $id | grep "submit_args" | awk 'BEGIN{FS="="}{print $2}'`
    set init_work_dir[$i]=`qstat -f -1 $id | grep "init_work_dir" | awk 'BEGIN{FS="="}{print $2}'`
    @ i++
end
@ i=1
while($i <= $jobnum)
#    echo "$JobId[$i] $job_state[$i] $queue[$i] $submit_args[$i] $init_work_dir[$i]"
    echo "$JobId[$i] $job_state[$i] $submit_args[$i] $init_work_dir[$i]"
    @ i++
end
