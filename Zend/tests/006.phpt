--TEST--
strncasecmp() tests
--FILE--
<?php

var_dump(strncasecmp(""));
var_dump(strncasecmp("", "", -1));
var_dump(strncasecmp("aef", "dfsgbdf", 0));
var_dump(strncasecmp("aef", "dfsgbdf", 10));
var_dump(strncasecmp("qwe", "qwer", 3));
var_dump(strncasecmp("qwerty", "QweRty", 6));
var_dump(strncasecmp("qwErtY", "qwer", 7));
var_dump(strncasecmp("q123", "Q123", 3));
var_dump(strncasecmp("01", "01", 1000));

echo "Done\n";
?>
--EXPECTF--
Warning: strncasecmp() expects exactly 3 parameters, 1 given in %s on line %d
NULL

Warning: Length must be greater than or equal to 0 in %s on line %d
bool(false)
int(0)
int(-3)
int(0)
int(0)
int(2)
int(0)
int(0)
Done
