#!/usr/bin/bash

#
# cs4130
# Project 01
# Sean McMeans
# ws1566sxm
# Due 23 Jan 2025  - accepted up to 10pm Jan24
# System = fry
# Compiler syntax = NA
# Job Control File = NA
# Additional File   = NA
# Results file = proj01.txt
#

function print_help() {
	echo "Usage: ./proj01.sh [numbers...]"
	echo "Calculates the sum, average, maximum, and minimum of the provided integers."
	echo "Options:"
	echo "  -h    Display this help message and exit"
	echo "  -v    Display version information and exit"
}

function print_version() {
	echo "proj01.sh version 1.0"
}

# Manual parsing for longer arguments
# Source: https://zerotomastery.io/blog/bash-getopts/
for arg in "$@"; do
	case $arg in
	-h|--help|-help|help|'?')
		print_help
		exit 0
		;;
	-v|--version|-version)
		print_version
		exit 0
		;;
	esac
done

# while getopts "hv" opt; do
# 	case ${opt} in
# 	h )
# 		print_help
# 		exit 0
# 		;;
# 	v )
# 		print_version
# 		exit 0
# 		;;
# 	\? )
# 		echo "Invalid option: -$OPTARG" >&2
# 		exit 1
# 		;;
# 	esac
# done


# Declare numbers array to hold passed arguments
declare -a numbers

# Get arguments passed
i=0
for number in "$@"
do
	numbers[i]=$number
	i=$((i+1))
done

# Convert empty arguments to 0 to not mess up math

# Array size resource
# https://stackoverflow.com/questions/43265069/explain-arrayname-syntax-for-array-length-in-bash
# for (( i=0; i<${#numbers[@]}; i++ ))
# do
# 	if [[ -z ${numbers[i]} ]]; then
# 		numbers[i]=0
# 	fi
# done

# Source for regex comparison
# https://stackoverflow.com/questions/17420994/how-can-i-match-a-string-with-a-regex-in-bash
function is_int() {
	reg_ex='^[+-]?[0-9]+$'
	if [[ $1 =~ $reg_ex ]]; then
		return 0
	else
		return 1
	fi
}

# Check if each number is a valid integer
for num in "${numbers[@]}"
do
	
	if ! is_int "$num"
	then
		echo "$num is not a valid input"
		print_help
		exit 1
	fi
done

# Calculate sum
sum=0
for num in "${numbers[@]}"
do
	sum=$((sum + num))
done

# Calculate average
count=${#numbers[@]}
average=$(echo "$sum / $count" | bc -l)

# Calculate max
max=${numbers[0]}
for num in "${numbers[@]}"
do
	if [ "$num" -gt "$max" ]; then
		max=$num
	fi
done

# Calculate min
min=${numbers[0]}
for num in "${numbers[@]}"
do
	if [ "$num" -lt "$min" ]; then
		min=$num
	fi
done

# Output results
printf "Sum: %d      " "$sum"
printf "Average: %.2f\n" "$average"
printf "Max: %d      " "$max"
printf "Min: %d\n" "$min"

# Add output to proj01.txt
{
	printf "proj01.sh %s\n" "$*"
	printf "Sum: %d      " "$sum"
	printf "Average: %.2f\n" "$average"
	printf "Max: %d      " "$max"
	printf "Min: %d\n\n" "$min"
} >> proj01.txt