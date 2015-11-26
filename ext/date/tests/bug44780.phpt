--TEST--
Bug #44780 (some time zone offsets not recognized by timezone_name_from_abbr)
--FILE--
<?php
var_dump( timezone_name_from_abbr("", 5.5*3600, false) );
var_dump( timezone_name_from_abbr("", 28800, false) );
?>
--EXPECT--
string(12) "Asia/Kolkata"
string(13) "Asia/Shanghai"
