--TEST--
Encoding INI test
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
default_charset=Shift_JIS
internal_encoding=
input_encoding=
output_encoding=
--FILE--
<?php
echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(mb_internal_encoding());

echo "Setting INI\n";
var_dump(ini_set('default_charset', 'UTF-8'));
var_dump(ini_set('internal_encoding', 'UTF-8'));
var_dump(ini_set('input_encoding', 'UTF-8'));
var_dump(ini_set('output_encoding', 'UTF-8'));

echo "Getting INI\n";
var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(mb_internal_encoding());
--EXPECT--
Getting INI
string(9) "Shift_JIS"
string(0) ""
string(0) ""
string(0) ""
string(4) "SJIS"
Setting INI
string(9) "Shift_JIS"
string(0) ""
string(0) ""
string(0) ""
Getting INI
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
