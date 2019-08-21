--TEST--
basic array_flip test
--FILE--
<?php
$trans = array("a" => 1,
               "b" => 1,
               "c" => 2,
               "z" => 0,
               "d" => TRUE,
               "E" => FALSE,
               "F" => NULL,
               0 => "G",
               1 => "h",
               2 => "i");

try {
    $trans = array_flip($trans);
} catch (\TypeError $e) {
    echo $e->getMessage() . "\n";
}
var_dump($trans);
?>

DONE
--EXPECT--
Can only flip STRING and INTEGER values!
array(10) {
  ["a"]=>
  int(1)
  ["b"]=>
  int(1)
  ["c"]=>
  int(2)
  ["z"]=>
  int(0)
  ["d"]=>
  bool(true)
  ["E"]=>
  bool(false)
  ["F"]=>
  NULL
  [0]=>
  string(1) "G"
  [1]=>
  string(1) "h"
  [2]=>
  string(1) "i"
}

DONE
