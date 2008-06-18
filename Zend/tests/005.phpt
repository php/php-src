--TEST--
strcasecmp() tests
--FILE--
<?php

var_dump(strcasecmp(""));
var_dump(strcasecmp("", ""));
var_dump(strcasecmp("aef", "dfsgbdf"));
var_dump(strcasecmp("qwe", "qwer"));
var_dump(strcasecmp("qwerty", "QweRty"));
var_dump(strcasecmp("qwErtY", "qwerty"));
var_dump(strcasecmp("q123", "Q123"));
var_dump(strcasecmp("01", "01"));

echo "Done\n";
?>
--EXPECTF--	
Warning: strcasecmp() expects exactly 2 parameters, 1 given in %s on line %d
NULL
int(0)
int(-3)
int(-1)
int(0)
int(0)
int(0)
int(0)
Done
