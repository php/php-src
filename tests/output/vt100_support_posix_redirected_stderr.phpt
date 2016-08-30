--TEST--
Test stream_vt100_support on POSIX with redirected STDERR
--SKIPIF--
<?php
if (!function_exists('posix_isatty')) {
    echo "skip Only for POSIX systems";
}
?>
--CAPTURE_STDIO--
STDERR
--FILE--
<?php 
ob_start();
var_dump(stream_vt100_support(STDOUT));
var_dump(stream_vt100_support(STDERR));
$content = ob_get_clean();
fwrite(STDERR, $content);
?>
--EXPECT--
bool(true)
bool(false)
