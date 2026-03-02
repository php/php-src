--TEST--
SplFileObject::fgetcsv with alternative enclosure
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv4.csv', 'w+');
fputcsv(
    $fp,
    [
        'field1',
        'field2',
        'field3',
        5,
    ],
    enclosure: '"',
    escape: '',
);
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv4.csv');
$fo->setCsvControl(escape: '');
var_dump($fo->fgetcsv(enclosure: '"'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv4.csv');
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
