--TEST--
GH-10187 (Segfault in stripslashes() with arm64)
--FILE--
<?php
var_dump(stripslashes("1234567890abcde\\"));
?>
--EXPECT--
string(15) "1234567890abcde"
