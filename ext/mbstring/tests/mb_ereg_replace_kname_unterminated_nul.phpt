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
--EXPECTF--
Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(12) "5c6b3c6e756d"

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(12) "5c6b276e756d"

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "615c6b3c6e"

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(8) "615c6bf0"

Deprecated: Function mb_ereg_replace() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
string(10) "5c5c6be282"
