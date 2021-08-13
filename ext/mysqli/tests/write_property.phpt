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

$driver->reconnect = 0;
var_dump($driver->reconnect);
$str = '4';
$str .= '2';
$driver->reconnect = $str;
var_dump($driver->reconnect);
try {
    $driver->reconnect = [];
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
bool(false)
bool(true)
Cannot assign array to property mysqli_driver::$reconnect of type bool
int(1)
Cannot assign array to property mysqli_driver::$report_mode of type int
