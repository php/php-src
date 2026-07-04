--TEST--
session rfc1867 freq: trailing whitespace after the "%" suffix is tolerated
--INI--
session.upload_progress.freq="5% "
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
--EXPECT--
string(2) "5%"
