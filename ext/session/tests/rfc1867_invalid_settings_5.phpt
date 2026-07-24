--TEST--
session rfc1867 invalid settings 5: whitespace-only freq is rejected
--INI--
session.upload_progress.freq="   "
error_log=
--EXTENSIONS--
session
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php
var_dump(ini_get("session.upload_progress.freq"));
?>
--EXPECTF--
Warning: PHP Startup: session.upload_progress.freq must be of type int in Unknown on line 0
string(%d) "1%"
