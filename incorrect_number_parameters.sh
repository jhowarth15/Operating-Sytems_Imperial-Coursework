./start 6 7 8
sleep 1
./producer &
./producer 2 &
./consumer 1 3
