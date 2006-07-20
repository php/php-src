--TEST--
json_encode() & endless loop - 1
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$a = array();
$a[] = &$a;

var_dump($a);
var_dump(json_encode($a));

echo "Done\n";
?>
--EXPECTF--	
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    &array(1) {
      [0]=>
      *RECURSION*
    }
  }
}

Catchable fatal error: json_encode(): recursion detected in %s on line %d
