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
Expected parameter 3 to be an array, int given
Expected parameter 4 to be an array, bool given
Expected parameter 5 to be an array, null given
