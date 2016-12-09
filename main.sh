#!/bin/sh
file=$1
firstLine=`head -n1 $file`
echo $firstLine | (grep '^[Ss]' >/dev/null && ./BDselect $file)
echo $firstLine | (grep '^[iI]' >/dev/null && ./BDinsert $file)
echo $firstLine | (grep '^[cC]' >/dev/null && ./BDcreate $file)


