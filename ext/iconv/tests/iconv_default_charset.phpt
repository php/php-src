--TEST--
Test default_charset handling
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_get_encoding') or die("skip iconv_get_encoding() is not available in this build");
?>
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
default_charset=UTF-8
internal_encoding=
input_encoding=
output_encoding=
iconv.internal_encoding=
iconv.input_encoding=
iconv.output_encoding=
--FILE--
<?php
echo "*** Testing default_charset handling ***\n";

echo "--- Get php.ini values ---\n";
var_dump(ini_get('default_charset'),
		 ini_get('internal_encoding'),
		 ini_get('input_encoding'),
		 ini_get('output_encoding'),
		 ini_get('iconv.internal_encoding'),
		 ini_get('iconv.input_encoding'),
		 ini_get('iconv.output_encoding'));

echo "\n--- Altering encodings ---\n";
var_dump(ini_set('default_charset', 'ISO-8859-1'));

echo "\n--- results of alterations ---\n";
var_dump(ini_get('default_charset'),
		 ini_get('internal_encoding'),
		 ini_get('input_encoding'),
		 ini_get('output_encoding'),
		 ini_get('iconv.internal_encoding'),
		 ini_get('iconv.input_encoding'),
		 ini_get('iconv.output_encoding'));

/*
echo "\n--- Altering encodings ---\n";
var_dump(ini_set('default_charset', 'ISO-8859-1'),
		 ini_set('internal_encoding'),
		 ini_set('input_encoding'),
		 ini_set('output_encoding'),
		 ini_set('iconv.internal_encoding'),
		 ini_set('iconv.input_encoding'),
		 ini_set('iconv.output_encoding'));
*/

echo "Done";
?>
--EXPECTF--
*** Testing default_charset handling ***
--- Get php.ini values ---
string(5) "UTF-8"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""

--- Altering encodings ---
string(5) "UTF-8"

--- results of alterations ---
string(10) "ISO-8859-1"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
Done
