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
PHP Warning:  PHP Startup: session.upload_progress.freq cannot be over 100% in %s

Warning: PHP Startup: session.upload_progress.freq cannot be over 100% in %s
string(%d) "1%"
