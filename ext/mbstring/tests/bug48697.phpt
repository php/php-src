--TEST--
Bug #48697 (mb_internal_encoding() value gets reset by parse_str() or mb_parse_str()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
error_reporting=E_ALL & ~E_DEPRECATED
--FILE--
<?php
ini_set('mbstring.internal_encoding', 'ISO-8859-15');
ini_set('mbstring.encoding_translation', true);
var_dump(mb_internal_encoding());
mb_internal_encoding('UTF-8');
var_dump(mb_internal_encoding());
parse_str('a=b');
var_dump(mb_internal_encoding());
mb_internal_encoding('UTF-8');
var_dump(mb_internal_encoding());
parse_str('a=b');
var_dump(mb_internal_encoding());
?>
--EXPECT--
string(11) "ISO-8859-15"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
