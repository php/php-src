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
try {
    var_dump($fo->fgetcsv(',', 'invalid'));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv5.csv');
?>
--EXPECT--
enclosure must be a character
