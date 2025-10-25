--TEST--
Non-null object cast: null throws TypeError
--FILE--
<?php

try {
    var_dump((!object) null);
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
TypeError: Cannot cast null to object
