--TEST--
Test that sapi_windows_vt100_support exists only on Windows
--SKIPIF--
<?php
if (stripos(PHP_OS, 'WIN') === 0) {
    echo "skip Only for not Windows systems";
}
?>
--FILE--
<?php
var_dump(function_exists('sapi_windows_vt100_support'));
?>
--EXPECT--
bool(false)
