--TEST--
json_encode() & endless loop - 1
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$a = array();
$a[] = &$a;

var_dump($a);

echo "\n";

var_dump(json_encode($a));
var_dump(json_last_error(), json_last_error_msg());

echo "\n";

var_dump(json_encode($a, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error(), json_last_error_msg());

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

bool(false)
int(6)
string(%d) "Recursion detected"

string(8) "[[null]]"
int(6)
string(%d) "Recursion detected"
Done
