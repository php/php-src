--TEST--
Test stream_vt100_support on newer Windows versions with redirected STDIO/STDERR
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
STDOUT, STDERR
--FILE--
<?php
var_dump(stream_vt100_support(STDOUT, true));
var_dump(stream_vt100_support(STDERR, true));
stream_vt100_support(STDOUT, false);
stream_vt100_support(STDERR, false);
?>
--EXPECT--
bool(false)
bool(false)
