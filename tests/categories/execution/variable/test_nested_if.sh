i=10
if [ $i == 10 ]
then
    i=9
    if [ $i == 10 ]
    then
        echo no
    else
        echo yes
    fi
else
    echo no2
fi
