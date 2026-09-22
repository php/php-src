--TEST--
GH-18642 (Signed integer overflow in ext/phar fseek)
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
require_once __DIR__ . '/files/phar_oo_test.inc';
class MyFile extends SplFileObject
{
}
$phar = new Phar($fname);
$phar->setInfoClass('MyFile');
$f = $phar['a.php'];
var_dump($f->fseek(PHP_INT_MAX));
var_dump($f->fseek(0));
var_dump($f->fseek(PHP_INT_MIN, SEEK_END));
var_dump($f->fseek(0, SEEK_SET));
var_dump($f->fseek(1, SEEK_CUR));
var_dump($f->fseek(PHP_INT_MAX, SEEK_CUR));
?>
--EXPECT--
int(-1)
int(0)
int(-1)
int(0)
int(0)
int(-1)
