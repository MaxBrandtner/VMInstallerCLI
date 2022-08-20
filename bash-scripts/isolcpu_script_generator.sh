#!/bin/bash
# cpu pinning works on a per core basis
# isolating a single thread in a multithreaded system doesn't improve performance within the vm
# $1 = selected_cores

selected_core_threads_string=""
n_cores=$(lscpu | grep 'Core(s)' | awk '{print $4}')
n_threads=$(lscpu | grep 'Thread(s)' | awk '{print $4}')

for (( i=1; i<=$1; i++))
do
	for (( j=0; j<$(lscpu | grep 'Thread(s)' | awk '{print $4}'); j++ ))
	do
		selected_core_thread=$((j * n_cores  -1 + i))
		#echo $selected_core_thread
		selected_core_threads_string=$(echo "$(echo "$selected_core_threads_string"),$(echo "$selected_core_thread")")
	done
done

#echo "$( echo "$selected_core_threads_string" | awk '{print substr($0,2)}')"

selected_core_threads_string=$(echo "$( echo "$selected_core_threads_string" | awk '{print substr($0,2)}')")
#echo $selected_core_threads_string


rm hooks/prepare/begin/isolstart.sh
touch "hooks/prepare/begin/isolstart.sh"
echo "systemctl set property --runtime -- user.slice AllowedCPUs=$selected_core_threads_string" >> hooks/prepare/begin/isolstart.sh
echo "systemctl set property --runtime -- system.slice AllowedCPUs=$selected_core_threads_string" >> hooks/prepare/begin/isolstart.sh
echo "systemctl set property --runtime -- init.scope AllowedCPUs=$selected_core_threads_string" >> hooks/prepare/begin/isolstart.sh

chmod +x hooks/prepare/begin/isolstart.sh


rm hooks/release/end/isolrevert.sh
touch "hooks/release/end/isolrevert.sh"
echo "systemctl set property --runtime -- user.slice AllowedCPUs=0-$(( n_cores * n_threads - 1))" >> hooks/release/end/isolrevert.sh
echo "systemctl set property --runtime -- system.slice AllowedCPUs=0-$(( n_cores * n_threads - 1))" >> hooks/release/end/isolrevert.sh
echo "systemctl set property --runtime -- init.scope AllowedCPUs=0-$(( n_cores * n_threads - 1))" >> hooks/release/end/isolrevert.sh

chmod +x hooks/release/end/isolrevert.sh
