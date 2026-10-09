--TEST--
Windows CLI INI conversion preserves ASCII names and values with UTF-7
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip Windows only');
?>
--INI--
internal_encoding=UTF-7
user_agent="foo+bar"
--FILE--
<?php
var_dump(ini_get('user_agent'));
var_dump(sapi_windows_cp_get());
?>
--EXPECT--
string(7) "foo+bar"
int(65000)
