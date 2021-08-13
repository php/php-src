--TEST--
Encoding INI test
--EXTENSIONS--
mbstring
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
default_charset=
internal_encoding=EUC-JP
input_encoding=
output_encoding=
mbstring.internal_encoding=
mbstring.http_input=
mbstring.http_output=
--FILE--
<?php
echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_get('mbstring.internal_encoding'));
var_dump(mb_internal_encoding());
var_dump(ini_get('mbstring.http_input'));
var_dump(ini_get('mbstring.http_output'));

echo "Setting INI\n";
var_dump(ini_set('default_charset', 'UTF-8'));
var_dump(ini_set('internal_encoding', 'UTF-8'));
var_dump(ini_set('input_encoding', 'UTF-8'));
var_dump(ini_set('output_encoding', 'UTF-8'));
var_dump(ini_set('mbstring.internal_encoding', 'UTF-8'));
var_dump(ini_set('mbstring.http_input', 'UTF-8'));
var_dump(ini_set('mbstring.http_output', 'UTF-8'));

echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_get('mbstring.internal_encoding'));
var_dump(mb_internal_encoding());
var_dump(ini_get('mbstring.http_input'));
var_dump(ini_get('mbstring.http_output'));
?>
--EXPECT--
Getting INI
string(0) ""
string(6) "EUC-JP"
string(0) ""
string(0) ""
string(0) ""
string(6) "EUC-JP"
string(0) ""
string(0) ""
Setting INI
string(0) ""
string(6) "EUC-JP"
string(0) ""
string(0) ""
string(0) ""
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
string(5) "UTF-8"
