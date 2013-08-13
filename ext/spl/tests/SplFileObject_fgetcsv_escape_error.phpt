--TEST--
SplFileObject::fgetcsv with alternative delimeter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv.csv', 'w+');
fwrite($fp, '"aaa","b""bb","ccc"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv.csv');
var_dump($fo->fgetcsv(',', '"', 'invalid'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv.csv');
?>
--EXPECTF--
Warning: SplFileObject::fgetcsv(): escape must be a character in %s on line %d
bool(false)
