--TEST--
Bug #72320 (iconv_substr returns false for empty strings)
--EXTENSIONS--
iconv
--FILE--
<?php
var_dump(iconv_substr('', 0, 10, 'UTF-8'));
var_dump(iconv_substr('foo', 3, 10, 'UTF-8'));
?>
--EXPECT--
string(0) ""
string(0) ""
