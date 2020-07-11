--TEST--
Test strlen() function : basic functionality
--FILE--
<?php

echo "*** Testing strlen() : basic functionality ***\n";
var_dump(strlen("abcdef"));
var_dump(strlen(" ab de "));
var_dump(strlen(""));
var_dump(strlen("\x90\x91\x00\x93\x94\x90\x91\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9d\x9e\x9f"));
?>
--EXPECT--
*** Testing strlen() : basic functionality ***
int(6)
int(7)
int(0)
int(18)
