./start 20
sleep 1
./producer 1 5 &
./producer 2 10 &
./producer 3 15 &
./consumer 1 &
./consumer 2 &
./producer 4 20 &
./consumer 3 &
./consumer 4 &
./consumer 5 &
./consumer 6 &
./producer 5 1 &
