--TEST--
session rfc1867 invalid settings
--INI--
session.upload_progress.freq=-1
error_log=
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php
var_dump(ini_get("session.upload_progress.freq"));
?>
--EXPECTF--
PHP Warning:  PHP Startup: session.upload_progress.freq must be greater than or equal to zero in %s

Warning: PHP Startup: session.upload_progress.freq must be greater than or equal to zero in %s
string(%d) "1%"
