for i in 1 2 3 4 5
do
    if [ $i == 2 ]
    then
        continue
    fi
    echo $i
done
