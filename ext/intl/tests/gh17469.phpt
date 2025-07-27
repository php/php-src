--TEST--
GH-17469: UConverter::transcode() raises always E_WARNING regardless of INI settings
--EXTENSIONS--
intl
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") die("skip currently unsupported on Windows");
?>
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.use_exceptions", 0);
UConverter::transcode("\x0a", 'nein!!', 'UTF-8');
UConverter::transcode("\x0a", 'UTF-16BE', 'da!');

ini_set("intl.error_level", 0);
ini_set("intl.use_exceptions", 1);

try {
	UConverter::transcode("\x0a", 'nein!!', 'UTF-8');
} catch (IntlException $e) {
	echo $e->getMessage(), PHP_EOL;
}
try {
	UConverter::transcode("\x0a", 'UTF-16BE', 'da!');
} catch (IntlException $e) {
	echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--

Warning: UConverter::transcode(): Error setting encoding: 4 - U_FILE_ACCESS_ERROR in %s on line %d

Warning: UConverter::transcode(): Error setting encoding: 4 - U_FILE_ACCESS_ERROR in %s on line 5
Error setting encoding: 4 - U_FILE_ACCESS_ERROR
Error setting encoding: 4 - U_FILE_ACCESS_ERROR
