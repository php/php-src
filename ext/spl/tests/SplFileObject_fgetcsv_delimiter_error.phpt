--TEST--
SplFileObject::fgetcsv with alternative delimiter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv3.csv', 'w+');
fputcsv($fp, array(
	'field1',
	'field2',
	'field3',
	5
), '|');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv3.csv');
var_dump($fo->fgetcsv('invalid'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv3.csv');
?>
--EXPECTF--
Warning: SplFileObject::fgetcsv(): delimiter must be a character in %s on line %d
bool(false)
