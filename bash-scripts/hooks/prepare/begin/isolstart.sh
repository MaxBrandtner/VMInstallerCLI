systemctl set property --runtime -- user.slice AllowedCPUs=0,6,1,7,2,8,3,9,4,10
systemctl set property --runtime -- system.slice AllowedCPUs=0,6,1,7,2,8,3,9,4,10
systemctl set property --runtime -- init.scope AllowedCPUs=0,6,1,7,2,8,3,9,4,10
