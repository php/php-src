--TEST--
Nullable int cast: basic functionality
--FILE--
<?php

var_dump((?int) null);
var_dump((?int) 123);
var_dump((?int) "456");
var_dump((?int) 78.9);
var_dump((?int) true);
var_dump((?int) false);

class SimpleObject {}
$obj = new SimpleObject();

try {
    var_dump((?int) $obj);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
NULL
int(123)
int(456)

Deprecated: Implicit conversion from float 78.9 to int loses precision in %s on line %d
int(78)
int(1)
int(0)
TypeError: Cannot cast SimpleObject to int
