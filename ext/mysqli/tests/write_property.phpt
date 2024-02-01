--TEST--
Writing to mysqli properties
--EXTENSIONS--
mysqli
--FILE--
<?php

$driver = new mysqli_driver;
try {
    /* Read-only property */
    $driver->client_info = 'test';
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$driver->report_mode = "1";
var_dump($driver->report_mode);
try {
    $driver->report_mode = [];
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Cannot write read-only property mysqli_driver::$client_info
int(1)
Cannot assign array to property mysqli_driver::$report_mode of type int
