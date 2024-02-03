i=0
while [ $i != 3 ]
do
    if [ $i == 0 ]
    then
        i=1
    elif [ $i == 1 ]
    then
        i=2
        continue
    else
        i=3
    fi
    echo $i
done
