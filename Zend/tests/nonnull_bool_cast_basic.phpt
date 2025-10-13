--TEST--
Non-null bool cast: basic functionality
--FILE--
<?php

var_dump((!bool) true);
var_dump((!bool) false);

var_dump((!bool) 0);
var_dump((!bool) 1);
var_dump((!bool) 42);

var_dump((!bool) "");
var_dump((!bool) "0");
var_dump((!bool) "hello");

class SimpleObject {}
$obj = new SimpleObject();

try {
    var_dump((!bool) $obj);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
bool(true)
TypeError: Cannot cast SimpleObject to bool
