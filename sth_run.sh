
dir=$1

rsync -av --delete /K/work/Dev/_forked/php-src/ .
cp $dir/zend_tmp_sth.h Zend

./buildconf
../conf --disable-phar

make clean
make

make test 2>&1 | tee $dir/test.log


