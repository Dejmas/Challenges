#!/bin/bash

for file_in in tests/*.in; do
	file_out="${file_in/in/out}";
	echo $file_out
	./a.out < "$file_in" | diff - "${file_out}" 2>&1 1>/dev/null
	if (( $? == 0 )); then 
		echo "$file_in ok";
	else 
		echo "$file_in different!";
		# cat $file_in;
		# echo "= student output:";
		# ./a.out < "$file_in";
		# echo "= reference output:";
		# cat "${file_out}";
		# echo "====";
	fi
done