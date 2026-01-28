--TEST--
Ensure mb_substr does not crash with MacJapanese input, when codepoints to skip are more than number of bytes in input string
--EXTENSIONS--
mbstring
--FILE--
<?php
var_dump(mb_substr("\x85\xAC", -1, encoding: 'MacJapanese'));
?>
--EXPECT--
string(1) "V"
