--TEST--
Assertion failure when fseeking outside of bounds of phar file
--EXTENSIONS--
phar
--INI--
phar.require_hash=0
--FILE--
<?php
require_once __DIR__ . '/files/phar_oo_test.inc';
$phar = new Phar($fname);
$phar->setInfoClass('SplFileObject');
$f = $phar['a.php'];
var_dump($f->fseek(1, SEEK_SET));
var_dump($f->fseek(999999, SEEK_SET));
var_dump($f->fseek(999999, SEEK_CUR));
var_dump($f->ftell());
var_dump($f->fseek(1, SEEK_CUR));
var_dump($f->fread(3));
?>
--EXPECT--
int(0)
int(-1)
int(-1)
int(1)
int(0)
string(3) "php"
