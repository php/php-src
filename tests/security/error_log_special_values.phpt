--TEST--
Setting error_log to special values with open_basedir enabled
--INI--
open_basedir=foo
error_log=
--FILE--
<?php
var_dump(ini_set("error_log", "syslog"));
var_dump(ini_set("error_log", ""));
?>
--EXPECT--
string(0) ""
string(6) "syslog"
