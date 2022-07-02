--TEST--
Bug #81298: mb_detect_encoding() segfaults when 7bit encoding is specified
--EXTENSIONS--
mbstring
--FILE--
<?php

var_dump(mb_detect_encoding("foobar.", "7bit"));
var_dump(mb_detect_encoding("foobar.", "7bit,ascii"));
var_dump(mb_detect_encoding("foobar.", "7bit,ascii,utf8"));
var_dump(mb_detect_encoding("foobar.", "html"));
var_dump(mb_detect_encoding("foobar.", "ascii,html"));

?>
--EXPECT--
bool(false)
string(5) "ASCII"
string(5) "ASCII"
bool(false)
string(5) "ASCII"
