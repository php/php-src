--TEST--
Test iconv_get_encoding()/iconv_set_encoding() function : basic functionality
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_get_encoding') or die("skip iconv_get_encoding() is not available in this build");
?>
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
iconv.input_encoding=ISO-8859-1
iconv.internal_encoding=ISO-8859-1
iconv.output_encoding=ISO-8859-1
--FILE--
<?php
/* Prototype  : mixed iconv_get_encoding([string type])
 * Description: Get internal encoding and output encoding for ob_iconv_handler() 
 * Prototype  : bool iconv_set_encoding(string type, string charset)
 * Description: Sets internal encoding and output encoding for ob_iconv_handler()
 * Source code: ext/iconv/iconv.c 
 */

/*
 * Test Basic functionality of iconv_get_encoding/iconv_set_encoding
 */

echo "*** Testing iconv_get_encoding()/iconv_set_encoding() : basic functionality ***\n";

echo "--- Default get_encoding ---\n";
var_dump(iconv_get_encoding());
var_dump(iconv_get_encoding("input_encoding"));
var_dump(iconv_get_encoding("output_encoding"));
var_dump(iconv_get_encoding("internal_encoding"));
var_dump(iconv_get_encoding("all"));

echo "\n--- Altering encodings ---\n";
var_dump(iconv_set_encoding("input_encoding", "UTF-8"));
var_dump(iconv_set_encoding("output_encoding", "UTF-8"));
var_dump(iconv_set_encoding("internal_encoding", "UTF-8"));


echo "\n--- results of alterations ---\n";
var_dump(iconv_get_encoding());
var_dump(iconv_get_encoding("input_encoding"));
var_dump(iconv_get_encoding("output_encoding"));
var_dump(iconv_get_encoding("internal_encoding"));
var_dump(iconv_get_encoding("all"));


echo "Done";
?>
--EXPECTF--
*** Testing iconv_get_encoding()/iconv_set_encoding() : basic functionality ***
--- Default get_encoding ---
array(3) {
  ["input_encoding"]=>
  string(10) "ISO-8859-1"
  ["output_encoding"]=>
  string(10) "ISO-8859-1"
  ["internal_encoding"]=>
  string(10) "ISO-8859-1"
}
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
array(3) {
  ["input_encoding"]=>
  string(10) "ISO-8859-1"
  ["output_encoding"]=>
  string(10) "ISO-8859-1"
  ["internal_encoding"]=>
  string(10) "ISO-8859-1"
}

--- Altering encodings ---
bool(true)
bool(true)
bool(true)

--- results of alterations ---
array(3) {
  ["input_encoding"]=>
  string(5) "UTF-8"
  ["output_encoding"]=>
  string(5) "UTF-8"
  ["internal_encoding"]=>
  string(5) "UTF-8"
}
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
array(3) {
  ["input_encoding"]=>
  string(5) "UTF-8"
  ["output_encoding"]=>
  string(5) "UTF-8"
  ["internal_encoding"]=>
  string(5) "UTF-8"
}
Done
