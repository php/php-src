--TEST--
PHP encoding setting test
--INI--
--FILE--
<?php
var_dump(ini_get('default_charset'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_set('default_charset', 'ISO-8859-1'));
var_dump(ini_get('default_charset'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(ini_set('input_encoding', 'ISO-8859-1'));
var_dump(ini_set('internal_encoding', 'ISO-8859-1'));
var_dump(ini_set('output_encoding', 'ISO-8859-1'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('output_encoding'));

--EXPECTF--
string(5) "UTF-8"
string(0) ""
string(0) ""
string(0) ""
string(5) "UTF-8"
string(10) "ISO-8859-1"
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
string(10) "ISO-8859-1"
