--TEST--
Bug #40501 (fgetcsv() can't handle trailing odd number of backslashes)
--FILE--
<?php
$file = __DIR__.'/bug40501.csv';

$h = fopen($file, 'r');
$data = fgetcsv($h, NULL, ',', '"', '"');
fclose($h);

var_dump($data);
?>
--EXPECTF--
array(2) {
  [0]=>
  string(%d) "this element contains the delimiter, and ends with an odd number of
backslashes (ex: 1)\"
  [1]=>
  string(%d) "and it isn't the last element$"
}
