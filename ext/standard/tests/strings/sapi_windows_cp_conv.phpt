--TEST--
sapi_windows_cp_conv basic functionality
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip for Windows only');
if (!sapi_windows_cp_set(1252) || !sapi_windows_cp_set(65001)) die('skip codepage not available');
?>
--FILE--
<?php
var_dump(
    bin2hex(sapi_windows_cp_conv(65001, 1252, 'äöü')),
    bin2hex(sapi_windows_cp_conv('utf-8', 1252, 'äöü')),
    bin2hex(sapi_windows_cp_conv(65001, 'windows-1252', 'äöü')),
    bin2hex(sapi_windows_cp_conv('utf-8', 'windows-1252', 'äöü')),
);
?>
--EXPECT--
string(6) "e4f6fc"
string(6) "e4f6fc"
string(6) "e4f6fc"
string(6) "e4f6fc"
