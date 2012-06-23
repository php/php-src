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
var_dump(json_last_error());

var_dump(json_encode($a, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error());

echo "Done\n";
?>
--EXPECTF--	
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}

Warning: json_encode(): recursion detected in %s on line %d
bool(false)
int(6)

Warning: json_encode(): recursion detected in %s on line %d
string(8) "[[null]]"
int(6)
Done
