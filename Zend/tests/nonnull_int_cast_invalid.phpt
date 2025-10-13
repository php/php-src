--TEST--
Non-null int cast: invalid string throws TypeError
--FILE--
<?php

try {
    var_dump((!int) "123aze");
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

try {
    var_dump((!int) "abc");
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECTF--
TypeError: Cannot cast string to int
TypeError: Cannot cast string to int
