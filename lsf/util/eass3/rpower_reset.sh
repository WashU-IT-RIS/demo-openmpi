#!/bin/sh

# For input:
# As part of convention for LSF calling this script
# arguments of shell are host list separated by blanks.

# For output:
# The out put of script should following LSF integration convention:
#      host1 0
#      host2 1
#      ...
#      hostn 0
# each host should comes from arguments
# one host per line: a host name with 0 if host power operation succeed
#                              , with 1 if power operation failed.
# rpower command format is rpower nodeA[,nodeB ...] [reset|suspend|wake ...]

# configFile is used to include some common configuration parameters, 
# such as the "config" file in the current directory. Absolute path is used for this parameter. 
# Please refer to each parameter for more detail.

#configFile="/home/..."
if [ $configFile ]; then
    . $configFile
fi
namelist=""
for arg in "$@"
do
    namelist="$namelist $arg"
done
namelist=`echo "$namelist" | sed "s/^ //"`

hostArray=($namelist)
nodeArray=($namelist)
rollBack=0

# map the host name to node name 
if [ $hostNodeMap ]; then
    lenMap=${#hostNodeMap[@]}  
    lenArray=${#nodeArray[@]}  
    i=0  
    while [ $i -lt $lenMap ]  
    do  
        j=0  
        while [ $j -lt $lenArray ]  
        do  
            if [ "${hostNodeMap[$i]}" = "${nodeArray[$j]}" ] ; then
                nodeArray[$j]=`echo ${hostNodeMap[ (( i+1 )) ]}`  
                break
            fi
            ((j++))
        done  
        ((i+=2))
    done  
    rollBack=1
fi

function updateResultHost()
{
    resultStrHost=$resultStrNode
    if [ "$rollBack" -eq 1 ]; then
        i=0
        lenMap=${#nodeArray[@]} 
        while [ $i -lt $lenMap ]  
        do  
            resultStrHost=`echo "$resultStrHost" | sed "s/${nodeArray[$i]}/${hostArray[$i]}/g"`  
            ((i++))
        done  
    fi
}


namelist=`echo ${nodeArray[*]} | sed "s/ /,/g"`

resumeCmd="rpower hostname wake"
resetCmd="rpower hostname reset"
queryCmd="rpower hostname stat"

#query before take action, if the host power status is already on, print ok
command=`echo "$queryCmd" | sed "s/hostname/$namelist/"`
resultStrNode=`$command 2>&1 < /dev/null`
updateResultHost

# handle this error output, 
# "Error: Invalid nodes and/or groups in noderange: node1,node2"
echo "$resultStrHost" | grep "\<Error\>" | grep "Invalid nodes and/or groups in noderange" | sed "s/Error\: Invalid nodes and\/or groups in noderange\: //g" | sed "s/$/;/g" | sed "s/,/ 1\n/g" | sed "s/;/ 1/g"
resultStrHost=`echo "$resultStrHost" | grep -v "Invalid nodes and/or groups in noderange"`


#reply the power operation fail when rpower reply a error 
echo "$resultStrHost" | grep "\<Error\>" | sed "s/\://" | awk '{print $1" 1"}'

#shrink the resultStrNode and resultStrHost
resultStrNode=`echo "$resultStrNode" | grep -v "\<Error\>"`
resultStrHost=`echo "$resultStrHost" | grep -v "\<Error\>"`

#check the result, if the host power status is on, print ok.
echo "$resultStrHost" | sed "s/\://" | awk '{if($2 == "on") {print $1" 0"}}'

#shrink the resultStrNode
resultStrNode=`echo "$resultStrNode" | grep -v "\<on\>[ ]*$"`

resultStrBak=$resultStrNode
#shrink the req hostname and send the rpower wake command if the host power status is suspend
namelist=`echo "$resultStrNode"  | grep "\<suspend\>[ ]*$" | sed "s/\:.*$/,/" | xargs | sed "s/,$//" | sed "s/, /,/g"`
if [ "$namelist" != "" ]; then
    command=`echo "$resumeCmd" | sed "s/hostname/$namelist/"`
    resultStrNode=`$command 2>&1 < /dev/null`
    updateResultHost
    echo "$resultStrHost" | sed "s/\://" | awk ' {if($2 == "wake"  ) {print $1" 0"} else {print $1" 1"} }'
fi

resultStrNode=$resultStrBak
#shrink the resultStr
resultStrNode=`echo "$resultStrNode" | grep -v "\<suspend\>[ ]*$"`

#shrink the req hostname and send the rpower command is reset
namelist=`echo "$resultStrNode"  | sed "s/\:.*$/,/" | xargs | sed "s/,$//" | sed "s/, /,/g"`
if [ "$namelist" != "" ]; then
    command=`echo "$resetCmd" | sed "s/hostname/$namelist/"`
    resultStrNode=`$command 2>&1 < /dev/null`
    updateResultHost
    echo "$resultStrHost" | sed "s/\://" | awk ' {if($2 == "reset"  ) {print $1" 0"} else {print $1" 1"} }'
fi
