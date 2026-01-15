--TEST--
Nullable float cast: basic functionality
--FILE--
<?php

var_dump((?float) null);
var_dump((?float) 12.34);
var_dump((?float) 56);
var_dump((?float) "78.9");
var_dump((?float) true);
var_dump((?float) false);

class SimpleObject {}
$obj = new SimpleObject();

try {
    var_dump((?float) $obj);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
NULL
float(12.34)
float(56)
float(78.9)
float(1)
float(0)
TypeError: Cannot cast SimpleObject to float
