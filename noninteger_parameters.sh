./start H
sleep 1
./producer CX 5 &
./producer 2 a &
./consumer % &
