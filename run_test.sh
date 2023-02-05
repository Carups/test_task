#!/bin/bash

for name in Array Heap Hash Map
do
   cmake . -DDataStructure=$name
   cmake --build .
   sum=0
   for((i=0;i<10;++i))
   do
       time_result=`./test_fast_forward_$name huobi_global_depth.log out_$name`
       num=`echo "${time_result//[!0-9]/}"`
       ((sum=sum+num))
       echo $time_result >> time_result_$name 
   done
   let avg=sum/10
   echo "Average time: $avg miliseconds" >> time_result_$name
done
