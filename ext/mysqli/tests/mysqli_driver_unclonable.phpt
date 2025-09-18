--TEST--
Trying to clone mysqli_driver object
--EXTENSIONS--
mysqli
--FILE--
<?php

try {
    $driver = new mysqli_driver;
    $driver_clone = clone $driver;
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
Error: Trying to clone an uncloneable object of class mysqli_driver
