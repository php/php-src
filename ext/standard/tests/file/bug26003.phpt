--TEST--
Bug #26003 (fgetcsv() is not binary-safe)
--FILE--
<?php
$fp = fopen(__DIR__.'/test3.csv', 'r');
var_dump(fgetcsv($fp, 4096));
?>
--EXPECTF--
array(2) {
  [0]=>
  string(3) "abc"
  [1]=>
  string(3) "d%0e"
}
