--TEST--
mb_detect_encoding() with incomplete trailing sequence
--EXTENSIONS--
mbstring
--FILE--
<?php

// Even in non-strict mode, this should detect as ISO-8859-1. When the end of the string is
// reached neither have illegal characters and would be picked based on score. However, flushing
// the string will disqualify UTF-8 due to illegal characters.
var_dump(mb_detect_encoding("A\xC2", ["UTF-8", "ISO-8859-1"]));

?>
--EXPECT--
string(10) "ISO-8859-1"
