--TEST--
Non-null int cast: null throws TypeError
--FILE--
<?php

try {
    var_dump((!int) null);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
TypeError: Cannot cast null to int
