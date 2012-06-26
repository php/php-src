--TEST--
json_encode() & endless loop - 2
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

$a = new stdclass;
$a->prop = $a;

var_dump($a);

var_dump(json_encode($a));
var_dump(json_last_error());

var_dump(json_encode($a, JSON_PARTIAL_OUTPUT_ON_ERROR));
var_dump(json_last_error());

echo "Done\n";
?>
--EXPECTF--	
object(stdClass)#%d (1) {
  ["prop"]=>
  *RECURSION*
}

Warning: json_encode(): recursion detected in %s on line %d
bool(false)
int(6)

Warning: json_encode(): recursion detected in %s on line %d
string(22) "{"prop":{"prop":null}}"
int(6)
Done
