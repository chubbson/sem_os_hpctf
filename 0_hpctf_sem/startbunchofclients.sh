# gnome-terminal -e ./client 
# sleep 1
i=1
while [ $i -le 100 ]
do
  echo $i
  ./client &
  sleep 1 
  i=`expr $i + 1`
done