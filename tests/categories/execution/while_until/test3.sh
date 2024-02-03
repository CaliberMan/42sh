i=0
until [ $i == 3 ]
do
    if [ $i == 0 ]
    then
        i=1
    elif [ $i == 1 ]
    then
        i=2
    else
        break
    fi
    echo $i
done
