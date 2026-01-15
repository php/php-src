--TEST--
Non-null array cast: null throws TypeError
--FILE--
<?php

try {
    var_dump((!array) null);
    echo "Should have thrown TypeError\n";
} catch (TypeError $e) {
    echo "TypeError: " . $e->getMessage() . "\n";
}

?>
--EXPECT--
TypeError: Cannot cast null to array
