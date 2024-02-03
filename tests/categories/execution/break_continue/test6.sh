for i in 1 2 3 4
do
    for j in 1 2 3 4
    do
        if [ $i == 3 ]
        then
            break 2
        fi
        echo $j
    done
    echo $i
done
