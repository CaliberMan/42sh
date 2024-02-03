a=1
a (){
    echo $1
}
a jaaj
echo $a
unset -f a
a jaaj
echo $a
unset -v a
