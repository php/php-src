--TEST--
Nullable object cast: invalid types throw TypeError
--FILE--
<?php

try {
    var_dump((?object) 123);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

try {
    var_dump((?object) "hello");
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
TypeError: Cannot cast int to object
TypeError: Cannot cast string to object
