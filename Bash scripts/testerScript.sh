FILE="./outputFile.csv"

echo "" > $FILE

loops=3
maxDepth=1000
internalloops=100

echo "Size,Serial,Parellel OpenMP,Parellel Vectorized,Parallel MPI" >> $FILE
filenames=("./MBS" "./MBSp" "./MBSpv" "mpirun -q -n 4 ./MBSmpi")
for i in {0..1000..10}
do
	start=$(($(date +%s%N)/1000000))
	printf "$i" >> $FILE
	echo -ne "$i\n"
	for program in "${filenames[@]}";do
	        sum=0
        	innerStart=$(($(date +%s%N)/1000000))
		for j in `seq 1 $loops`;do
			
                	tmp=$($program $maxDepth $i 2>&1)
                	sum=$(awk -v s="$sum" -v t="$tmp" 'BEGIN {s += t;print s}')
			echo -ne "\r$program ->\t\t["
			for k in `seq 1 $loops`;
			do
				if (($k > $j))
				then
					echo -ne "_"
				else
					echo -ne "#"
				fi				
			done
			echo -ne "]"
        	done
		innerEnd=$(($(date +%s%N)/1000000))
		innerRuntime=$((innerEnd-innerStart))
		echo -ne "$innerRuntime\n"
        	output=$(awk -v s="$sum" -v n="$loops" 'BEGIN {s /= n;print s}')
	        printf ",$output">>$FILE
	done
	end=$(($(date +%s%N)/1000000))
	runtime=$((end-start))
	echo "" >> $FILE
	echo -ne "runtime: $runtime \n\n"
	
done
