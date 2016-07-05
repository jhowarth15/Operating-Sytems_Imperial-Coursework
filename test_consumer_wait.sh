//To test consumer starts up again while waiting if jobs produced within 10sec.

//Start in one shell
./start 5
sleep 1
./consumer 1 

//Wait 7sec then start in another shell
./producer 1 3

