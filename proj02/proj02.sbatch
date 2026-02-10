#!/bin/bash
#SBATCH --job-name=decrypt
#SBATCH --output=task_%a.tmp
#SBATCH -N 1
#SBATCH --ntasks=1
#SBATCH --array=1-16

number="w1566sxm"
increment=$SLURM_ARRAY_TASK_ID

for (( i=$increment ; i<=64 ; i+=16 ));
do
	for (( j=1 ; j <=128 ; j++ ));
	do
		for (( k=1 ; k <=128 ; k++ ));
		do
			./decrypt $i $j $k $number
		done
	done

done	

