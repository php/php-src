--TEST--
json_encode() & endless loop - 3
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php

$a = array();
$a[] = $a;

var_dump($a);
var_dump(json_encode($a));

echo "Done\n";
?>
--EXPECTF--
array(1) {
  [0]=>
  array(0) {
  }
}
string(4) "[[]]"
Done
