--TEST--
Test stream_vt100_support on POSIX with redirected STDOUT
--SKIPIF--
<?php
if (!function_exists('posix_isatty')) {
    echo "skip Only for POSIX systems";
}
?>
--CAPTURE_STDIO--
STDOUT
--FILE--
<?php 
var_dump(stream_vt100_support(STDOUT));
var_dump(stream_vt100_support(STDERR));
?>
--EXPECT--
bool(false)
bool(true)
