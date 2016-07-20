--TEST--
SplFileObject::fgetcsv default path
--FILE--
<?php
$csv = <<<CSV
"field1","field2","field3"
"field
4","field
5","field6
"

CSV;
file_put_contents('SplFileObject__fgetcsv_dropnewline.csv', $csv);

// Read the CSV without DROP_NEW_LINES first
$fo = new SplFileObject('SplFileObject__fgetcsv_dropnewline.csv');
$fo->setFlags(SplFileObject::READ_CSV);
var_dump($fo->fgetcsv());
var_dump($fo->fgetcsv());

// Read the CSV without DROP_NEW_LINES first
$fo = new SplFileObject('SplFileObject__fgetcsv_dropnewline.csv');
$fo->setFlags(SplFileObject::READ_CSV | SplFileObject::DROP_NEW_LINE);
var_dump($fo->fgetcsv());
var_dump($fo->fgetcsv());
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv_dropnewline.csv');
?>
--EXPECTF--
array(3) {
  [0]=>
  string(6) "field1"
  [1]=>
  string(6) "field2"
  [2]=>
  string(6) "field3"
}
array(3) {
  [0]=>
  string(7) "field
4"
  [1]=>
  string(7) "field
5"
  [2]=>
  string(7) "field6
"
}
array(3) {
  [0]=>
  string(6) "field1"
  [1]=>
  string(6) "field2"
  [2]=>
  string(6) "field3"
}
array(3) {
  [0]=>
  string(7) "field
4"
  [1]=>
  string(7) "field
5"
  [2]=>
  string(7) "field6
"
}