--TEST--
Call internal function with incorrect number of arguments
--FILE--
<?php
try {
    substr("foo");
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
substr(): At least 2 arguments are expected, 1 given
