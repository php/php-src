--TEST--
Test stream_vt100_support for STDERR on newer Windows versions
--SKIPIF--
<?php
if (stripos(PHP_OS, 'WIN') !== 0) {
    echo "skip Only for Windows systems";
} elseif (version_compare(
    PHP_WINDOWS_VERSION_MAJOR.'.'.PHP_WINDOWS_VERSION_MINOR.'.'.PHP_WINDOWS_VERSION_BUILD,
	 '10.0.10586'
) < 0) {
	echo "skip Only for Windows systems >= 10.0.10586";
}
?>
--CAPTURE_STDIO--
STDOUT
--FILE--
<?php 
var_dump(stream_vt100_support(STDERR, true));
var_dump(stream_vt100_support(STDERR));
var_dump(stream_vt100_support(STDERR, false));
var_dump(stream_vt100_support(STDERR));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
