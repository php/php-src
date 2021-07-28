--TEST--
Bug #81298: mb_detect_encoding() segfaults when 7bit encoding is specified
--FILE--
<?php

var_dump(mb_detect_encoding("foobar", "7bit"));
var_dump(mb_detect_encoding("foobar", "7bit,ascii"));

?>
--EXPECT--
bool(false)
string(5) "ASCII"
