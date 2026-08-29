--TEST--
mb_ereg_replace() with unterminated \k<name> backref must not embed a NUL byte
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(bin2hex(mb_ereg_replace('(\d+)', '\k<num', '123')));
var_dump(bin2hex(mb_ereg_replace('(\d+)', "\\k'num", '123')));
var_dump(bin2hex(mb_ereg_replace('(x)(y)', 'a\k<n', 'xy')));
var_dump(bin2hex(mb_ereg_replace('(\d+)', "a\\k\xF0", '123')));
var_dump(bin2hex(mb_ereg_replace('(\d+)', "\\\\k\xE2\x82", '123')));
?>
--EXPECT--
string(12) "5c6b3c6e756d"
string(12) "5c6b276e756d"
string(10) "615c6b3c6e"
string(8) "615c6bf0"
string(10) "5c5c6be282"
