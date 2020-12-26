--TEST--
Test that max_execution_wall_time is an alias of max_execution_time on Unix-based platforms
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") {
    die('skip not for Windows');
}
?>
--INI--
max_execution_time=0
max_execution_wall_time=0
--FILE--
<?php

ini_set("max_execution_wall_time", 1);

var_dump(ini_get("max_execution_wall_time"));
var_dump(ini_get("max_execution_time"));

ini_set("max_execution_time", 2);

var_dump(ini_get("max_execution_wall_time"));
var_dump(ini_get("max_execution_time"));

?>
--EXPECTF--
string(1) "1"
string(1) "0"
string(1) "1"
string(1) "2"
