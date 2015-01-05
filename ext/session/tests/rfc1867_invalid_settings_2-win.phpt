--TEST--
session rfc1867 invalid settings 2
--INI--
session.upload_progress.freq=200%
error_log=
--SKIPIF--
<?php 
include('skipif.inc');
if(substr(PHP_OS, 0, 3) != "WIN")
        die("skip windows only test");
?>
--FILE--
<?php
var_dump(ini_get("session.upload_progress.freq"));
?>
--EXPECTF--
Warning: PHP Startup: session.upload_progress.freq cannot be over 100% in %s
string(2) "1%"
PHP Warning:  PHP Startup: session.upload_progress.freq cannot be over 100% in %s
