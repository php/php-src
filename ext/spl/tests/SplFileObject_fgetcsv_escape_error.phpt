--TEST--
SplFileObject::fgetcsv with alternative delimeter
--FILE--
<?php
$fp = fopen('SplFileObject::fgetcsv.csv', 'w+');
fwrite($fp, '"aaa","b""bb","ccc"');
fclose($fp);

$fo = new SplFileObject('SplFileObject::fgetcsv.csv');
var_dump($fo->fgetcsv(',', '"', 'invalid'));
?>
--CLEAN--
<?php
unlink('SplFileObject::fgetcsv.csv');
?>
--EXPECTF--
Warning: SplFileObject::fgetcsv(): escape must be a character in %s on line %d
bool(false)
