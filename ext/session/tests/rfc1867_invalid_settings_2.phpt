--TEST--
session rfc1867 invalid settings 2
--INI--
session.upload_progress.freq=200%
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
Warning: PHP Startup: session.upload_progress.freq must be less than or equal to 100% in Unknown on line 0
string(%d) "1%"
