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
substr() expects at least 2 arguments, 1 given
