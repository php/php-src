--TEST--
Bug #67988 (htmlspecialchars() does not respect default_charset specified by ini_set)
--INI--
default_charset=UTF-8
--FILE--
<?php
ini_set('default_charset', 'cp1252');

var_dump(ini_get('default_charset'));
var_dump(ini_get('internal_encoding'));
var_dump(ini_get('input_encoding'));
var_dump(ini_get('output_encoding'));

var_dump(htmlentities("\xA3", ENT_HTML5));
var_dump(htmlentities("\xA3", ENT_HTML5, 'cp1252'));

var_dump(bin2hex(html_entity_decode("&pound;", ENT_HTML5)));
var_dump(bin2hex(html_entity_decode("&pound;", ENT_HTML5, 'cp1252')));
--EXPECT--
string(6) "cp1252"
string(0) ""
string(0) ""
string(0) ""
string(7) "&pound;"
string(7) "&pound;"
string(2) "a3"
string(2) "a3"
