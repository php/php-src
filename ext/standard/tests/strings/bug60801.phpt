--TEST--
Bug #60801 (strpbrk() mishandles NUL byte)
--FILE--
<?php
$haystack = "foob\x00ar";
$needle = "a\x00b";

var_dump(strpbrk($haystack, 'ar'));
var_dump(strpbrk($haystack, "\x00"));
var_dump(strpbrk($haystack, $needle));
var_dump(strpbrk('foobar', $needle));
var_dump(strpbrk("\x00", $needle));
var_dump(strpbrk('xyz', $needle));
var_dump(strpbrk($haystack, 'xyz'));
?>
--EXPECTF--
string(2) "ar"
string(3) "%0ar"
string(4) "b%0ar"
string(3) "bar"
string(1) "%0"
bool(false)
bool(false)
