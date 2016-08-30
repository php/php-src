--TEST--
Test stream_vt100_support for STDOUT on newer Windows versions
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
STDERR
--FILE--
<?php
ob_start();
var_dump(stream_vt100_support(STDOUT, true));
var_dump(stream_vt100_support(STDOUT));
var_dump(stream_vt100_support(STDOUT, false));
var_dump(stream_vt100_support(STDOUT));
$content = ob_get_clean();
fwrite(STDERR, $content);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(false)
