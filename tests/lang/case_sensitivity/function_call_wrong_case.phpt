--TEST--
Function called with wrong case fails
--FILE--
<?php
try {
    $result = STRLEN("hello");
    echo $result . "\n";
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Call to undefined function STRLEN()
