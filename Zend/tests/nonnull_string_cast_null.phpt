--TEST--
Non-null string cast: null throws TypeError
--FILE--
<?php

try {
    var_dump((!string) null);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
TypeError: Cannot cast null to string
