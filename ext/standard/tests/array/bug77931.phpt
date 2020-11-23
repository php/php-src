--TEST--
Bug #77931: Warning for array_map mentions wrong type
--FILE--
<?php

try {
    array_map('trim', array(), 1);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    array_map('trim', array(), array(), true);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    array_map('trim', array(), array(), array(), null);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
array_map(): Argument #3 must be of type array, int given
array_map(): Argument #4 must be of type array, bool given
array_map(): Argument #5 must be of type array, null given
