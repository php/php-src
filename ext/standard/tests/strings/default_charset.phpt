--TEST--
default_charset and htmlentities/htmlspecialchars/html_entity_decode
--INI--
default_charset=UTF-8
internal_encoding=
--FILE--
<?php
ini_set('default_charset', 'cp1252');
var_dump(ini_get('default_charset'), ini_get('internal_encoding'));

var_dump(htmlentities("\xA3", ENT_HTML5));
var_dump(htmlentities("\xA3", ENT_HTML5, 'cp1252'));

var_dump(bin2hex(html_entity_decode("&pound;", ENT_HTML5)));
var_dump(bin2hex(html_entity_decode("&pound;", ENT_HTML5, 'cp1252')));
?>
--EXPECT--
string(6) "cp1252"
string(0) ""
string(7) "&pound;"
string(7) "&pound;"
string(2) "a3"
string(2) "a3"
