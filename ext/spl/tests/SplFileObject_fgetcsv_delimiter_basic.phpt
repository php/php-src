--TEST--
SplFileObject::fgetcsv with alternative delimiter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv2.csv', 'w+');
fputcsv($fp, array(
    'field1',
    'field2',
    'field3',
    5
), '|');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv2.csv');
var_dump($fo->fgetcsv('|'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv2.csv');
?>
--EXPECT--
array(4) {
  [0]=>
  string(6) "field1"
  [1]=>
  string(6) "field2"
  [2]=>
  string(6) "field3"
  [3]=>
  string(1) "5"
}
