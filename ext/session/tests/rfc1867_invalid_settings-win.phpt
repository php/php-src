--TEST--
session rfc1867 invalid settings
--INI--
session.upload_progress.freq=-1
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
Warning: PHP Startup: session.upload_progress.freq must be greater than or equal to zero in %s
string(2) "1%"
PHP Warning:  PHP Startup: session.upload_progress.freq must be greater than or equal to zero in %s
