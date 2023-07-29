--TEST--
Writing to mysqli_driver properties (strict_types)
--EXTENSIONS--
mysqli
--FILE--
<?php

declare(strict_types=1);

$driver = new mysqli_driver;
try {
    /* Read-only property */
    $driver->client_info = 42;
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

try {
    $driver->report_mode = "1";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot write read-only property mysqli_driver::$client_info
Cannot assign string to property mysqli_driver::$report_mode of type int
