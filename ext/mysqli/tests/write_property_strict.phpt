--TEST--
Writing to mysqli properties (strict_types)
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
    $driver->reconnect = 0;
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
Cannot assign int to property mysqli_driver::$reconnect of type bool
Cannot assign string to property mysqli_driver::$report_mode of type int
