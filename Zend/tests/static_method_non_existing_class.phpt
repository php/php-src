--TEST--
Calling a static method on a non-existing class
--FILE--
<?php

$str = "foo";
try {
    Test::{$str . "bar"}();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Class "Test" not found
