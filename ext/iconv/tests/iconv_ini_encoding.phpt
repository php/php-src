--TEST--
Encoding INI test
--EXTENSIONS--
iconv
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
default_charset=ISO-8859-1
internal_encoding=
input_encoding=
output_encoding=
iconv.internal_encoding=ISO-8859-1
iconv.http_input=ISO-8859-1
iconv.http_output=ISO-8859-1
--FILE--
<?php
echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_get('iconv.internal_encoding'));
var_dump(ini_get('iconv.input_encoding'));
var_dump(ini_get('iconv.output_encoding'));

echo "Setting INI\n";
var_dump(ini_set('default_charset', 'UTF-8'));
var_dump(ini_set('internal_encoding', 'UTF-8'));
var_dump(ini_set('input_encoding', 'UTF-8'));
var_dump(ini_set('output_encoding', 'UTF-8'));
var_dump(ini_set('iconv.internal_encoding', 'UTF-8'));
var_dump(ini_set('iconv.input_encoding', 'UTF-8'));
var_dump(ini_set('iconv.output_encoding', 'UTF-8'));

echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_get('iconv.internal_encoding'));
var_dump(ini_get('iconv.input_encoding'));
var_dump(ini_get('iconv.output_encoding'));
?>
--EXPECT--
Getting INI
string(10) "ISO-8859-1"
string(0) ""
string(0) ""
string(0) ""
string(10) "ISO-8859-1"
string(0) ""
string(0) ""
Setting INI
string(10) "ISO-8859-1"
string(0) ""
string(0) ""
string(0) ""
string(10) "ISO-8859-1"
string(0) ""
string(0) ""
Getting INI
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
