--TEST--
Bug #73988 (zone offset not recognized by timezone_name_from_abbr)
--FILE--
<?php
var_dump( timezone_name_from_abbr("", 6*3600, false) );
var_dump( timezone_name_from_abbr("", 6*3600, 0) );
var_dump( timezone_name_from_abbr("", 6*3600, 1) );
?>
--EXPECT--
string(16) "Asia/Novosibirsk"
string(16) "Asia/Novosibirsk"
string(18) "Asia/Yekaterinburg"
