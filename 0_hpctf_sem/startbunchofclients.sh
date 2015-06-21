# gnome-terminal -e ./client 
# sleep 1
i=1
while [ $i -le 300 ]
do
  echo $i
  ./client -ms=5 &
  sleep 1 
  i=`expr $i + 1`
done