--TEST--
Calling a static method on a non-existing class
--FILE--
<?php

$str = "foo";
try {
    Test::{$str . "bar"}();
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Class "Test" not found
