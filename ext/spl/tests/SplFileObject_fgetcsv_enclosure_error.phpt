--TEST--
SplFileObject::fgetcsv with alternative delimiter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv5.csv', 'w+');
fputcsv($fp, array(
	'field1',
	'field2',
	'field3',
	5
), ',', '"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv5.csv');
var_dump($fo->fgetcsv(',', 'invalid'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv5.csv');
?>
--EXPECTF--
Warning: SplFileObject::fgetcsv(): enclosure must be a character in %s on line %d
bool(false)
