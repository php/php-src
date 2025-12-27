--TEST--
Nullable array cast: invalid types throw TypeError
--FILE--
<?php

try {
    var_dump((?array) 123);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

try {
    var_dump((?array) "hello");
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
TypeError: Cannot cast int to array
TypeError: Cannot cast string to array
