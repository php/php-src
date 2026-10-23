--TEST--
sapi_windows_cp_set() and sapi_windows_cp_conv() must reject a negative codepage
--SKIPIF--
<?php
if (PHP_OS_FAMILY !== 'Windows') die('skip for Windows only');
?>
--FILE--
<?php
try {
    sapi_windows_cp_set(-1);
} catch (ValueError $exception) {
    echo $exception->getMessage(), "\n";
}

try {
    sapi_windows_cp_set(PHP_INT_MIN);
} catch (ValueError $exception) {
    echo $exception->getMessage(), "\n";
}

try {
    sapi_windows_cp_conv(-1, 65001, 'php');
} catch (ValueError $exception) {
    echo $exception->getMessage(), "\n";
}

try {
    sapi_windows_cp_conv(65001, -1, 'php');
} catch (ValueError $exception) {
    echo $exception->getMessage(), "\n";
}
?>
--EXPECT--
sapi_windows_cp_set(): Argument #1 ($codepage) must be between 0 and 4294967295
sapi_windows_cp_set(): Argument #1 ($codepage) must be between 0 and 4294967295
sapi_windows_cp_conv(): Argument #1 ($in_codepage) must be between 0 and 4294967295
sapi_windows_cp_conv(): Argument #2 ($out_codepage) must be between 0 and 4294967295
