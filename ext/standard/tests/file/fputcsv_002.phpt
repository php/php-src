--TEST--
fputcsv(): Checking data after calling the function
--FILE--
<?php

$file = dirname(__FILE__) .'/fgetcsv-test.csv';

$data = array(1, 2, 'foo', 'haha', array(4, 5, 6), 1.3, null);

$fp = fopen($file, 'w');

fputcsv($fp, $data);

var_dump($data);

@unlink($file);

?>
--EXPECTF--
Notice: Array to string conversion in %s on line %d
array(7) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  string(3) "foo"
  [3]=>
  string(4) "haha"
  [4]=>
  array(3) {
    [0]=>
    int(4)
    [1]=>
    int(5)
    [2]=>
    int(6)
  }
  [5]=>
  float(1.3)
  [6]=>
  NULL
}
