--TEST--
Non-null float cast: null throws TypeError
--FILE--
<?php

try {
    var_dump((!float) null);
    echo "Should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
TypeError: Cannot cast null to float
