--TEST--
Encoding INI test
--EXTENSIONS--
mbstring
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
default_charset=Shift_JIS
internal_encoding=
input_encoding=
output_encoding=
mbstring.internal_encoding=Shift_JIS
mbstring.http_output=Shift_JIS
--FILE--
<?php
echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_get('mbstring.internal_encoding'));
var_dump(mb_internal_encoding());
var_dump(ini_get('mbstring.http_output'));

echo "Setting INI\n";
var_dump(ini_set('default_charset', 'UTF-8'));
var_dump(ini_set('internal_encoding', 'UTF-8'));
var_dump(ini_set('input_encoding', 'UTF-8'));
var_dump(ini_set('output_encoding', 'UTF-8'));
var_dump(ini_set('mbstring.internal_encoding', 'UTF-8'));
var_dump(ini_set('mbstring.http_output', 'UTF-8'));

echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_get('mbstring.internal_encoding'));
var_dump(mb_internal_encoding());
var_dump(ini_get('mbstring.http_output'));
?>
--EXPECT--
Getting INI
string(9) "Shift_JIS"
string(0) ""
string(0) ""
string(0) ""
string(9) "Shift_JIS"
string(4) "SJIS"
string(9) "Shift_JIS"
Setting INI
string(9) "Shift_JIS"
string(0) ""
string(0) ""
string(0) ""
string(9) "Shift_JIS"
string(9) "Shift_JIS"
Getting INI
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
