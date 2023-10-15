--TEST--
Bug #48697 (mb_internal_encoding() value gets reset by parse_str() or mb_parse_str()
--EXTENSIONS--
mbstring
--FILE--
<?php
ini_set('mbstring.encoding_translation', true);
var_dump(mb_internal_encoding());
mb_internal_encoding('UTF-8');
var_dump(mb_internal_encoding());
parse_str('a=b', $ary);
var_dump(mb_internal_encoding());
mb_internal_encoding('UTF-8');
var_dump(mb_internal_encoding());
parse_str('a=b', $ary);
var_dump(mb_internal_encoding());
?>
--EXPECTF--
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"
