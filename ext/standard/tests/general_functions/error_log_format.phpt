--TEST--
error_date_format ini variable
--INI--
error_date_format=d.m.Y
log_errors=On
--FILE--
<?php

$dir = dirname(__FILE__);
$log = $dir . "/tmp.err";
ini_set("error_log", $log);
error_log("dummy");
readfile($log);
unlink($log);
--EXPECTF--
[%d.%d.%d] dummy
