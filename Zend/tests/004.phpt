--TEST--
strncmp() tests
--FILE--
<?php

var_dump(strncmp("", ""));
var_dump(strncmp("", "", 100));
var_dump(strncmp("aef", "dfsgbdf", -1));
var_dump(strncmp("fghjkl", "qwer", 0));
var_dump(strncmp("qwerty", "qwerty123", 6));
var_dump(strncmp("qwerty", "qwerty123", 7));

echo "Done\n";
?>
--EXPECTF--
Warning: strncmp() expects exactly 3 parameters, 2 given in %s on line %d
NULL
int(0)

Warning: Length must be greater than or equal to 0 in %s on line %d
bool(false)
int(0)
int(0)
int(-1)
Done
