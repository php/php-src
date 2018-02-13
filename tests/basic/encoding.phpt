--TEST--
PHP encoding setting test
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

var_dump(ini_set('input_encoding', 'EUC-JP'));
var_dump(ini_set('internal_encoding', 'EUC-JP'));
var_dump(ini_set('output_encoding', 'EUC-JP'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('output_encoding'));
?>
--EXPECT--
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
string(6) "EUC-JP"
string(6) "EUC-JP"
string(6) "EUC-JP"
