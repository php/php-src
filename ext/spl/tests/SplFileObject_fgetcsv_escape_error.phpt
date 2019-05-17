--TEST--
SplFileObject::fgetcsv with alternative delimiter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv8.csv', 'w+');
fwrite($fp, '"aaa","b""bb","ccc"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv8.csv');
var_dump($fo->fgetcsv(',', '"', 'invalid'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv8.csv');
?>
--EXPECTF--
Warning: SplFileObject::fgetcsv(): escape must be empty or a single character in %s on line %d
bool(false)
