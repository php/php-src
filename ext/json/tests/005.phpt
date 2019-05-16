--TEST--
json_encode() & endless loop - 3
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$a = array();
$a[] = $a;

var_dump($a);
var_dump(json_encode($a));

echo "Done\n";
?>
--EXPECT--
array(1) {
  [0]=>
  array(0) {
  }
}
string(4) "[[]]"
Done
