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

echo "Done\n";
?>
--EXPECTF--	
object(stdClass)#%d (1) {
  ["prop"]=>
  object(stdClass)#%d (1) {
    ["prop"]=>
    *RECURSION*
  }
}

Catchable fatal error: json_encode(): recursion detected in %s on line %d
