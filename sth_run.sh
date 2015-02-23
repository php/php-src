
dir=$1

make clean

cp $dir/zend_tmp_sth.h Zend

make $MAKEFLAGS

make test 2>&1 | tee $dir/test.log

