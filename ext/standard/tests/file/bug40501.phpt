--TEST--
Bug #40501 (fgetcsv() can't handle trailing odd number of backslashes)
--FILE--
<?php
$file = dirname(__FILE__).'/bug40501.csv';

$h = fopen($file, 'r');
$data = fgetcsv($h, NULL, ',', '"', '"');
fclose($h);

var_dump($data);
?>
--EXPECT--
array(2) {
  [0]=>
  string(88) "this element contains the delimiter, and ends with an odd number of
backslashes (ex: 1)\"
  [1]=>
  string(30) "and it isn't the last element$"
}
