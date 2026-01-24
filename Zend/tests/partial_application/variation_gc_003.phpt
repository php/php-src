--TEST--
PFA variation: GC (003)
--FILE--
<?php

function test(...$args) {
}

$obj = new stdClass;
$obj->prop = test(?, x: $obj);

echo "OK";
?>
--EXPECT--
OK
