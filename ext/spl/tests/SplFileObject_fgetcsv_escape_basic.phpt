--TEST--
SplFileObject::fgetcsv with alternative delimeter
--FILE--
<?php
$fp = fopen('SplFileObject__fgetcsv.csv', 'w+');
fwrite($fp, '"aaa","b""bb","ccc"');
fclose($fp);

$fo = new SplFileObject('SplFileObject__fgetcsv.csv');
var_dump($fo->fgetcsv(',', '"', '"'));
?>
--CLEAN--
<?php
unlink('SplFileObject__fgetcsv.csv');
?>
--EXPECTF--
array(3) {
  [0]=>
  string(3) "aaa"
  [1]=>
  string(4) "b"bb"
  [2]=>
  string(3) "ccc"
}
