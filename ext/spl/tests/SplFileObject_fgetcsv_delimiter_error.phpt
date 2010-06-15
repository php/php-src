--TEST--
SplFileObject::fgetcsv with alternative delimeter
--FILE--
<?php
$fp = fopen('SplFileObject::fgetcsv.csv', 'w+');
fputcsv($fp, array(
	'field1',
	'field2',
	'field3',
	5
), '|');
fclose($fp);

$fo = new SplFileObject('SplFileObject::fgetcsv.csv');
var_dump($fo->fgetcsv('invalid'));
?>
--CLEAN--
<?php
unlink('SplFileObject::fgetcsv.csv');
?>
--EXPECTF--
Warning: SplFileObject::fgetcsv(): delimiter must be a character in %s on line %d
bool(false)
