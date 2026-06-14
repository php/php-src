--TEST--
strncasecmp() tests
--FILE--
<?php

try {
    var_dump(strncasecmp("", "", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

var_dump(strncasecmp("aef", "dfsgbdf", 0));
var_dump(strncasecmp("aef", "dfsgbdf", 10));
var_dump(strncasecmp("qwe", "qwer", 3));
var_dump(strncasecmp("qwerty", "QweRty", 6));
var_dump(strncasecmp("qwErtY", "qwer", 7));
var_dump(strncasecmp("q123", "Q123", 3));
var_dump(strncasecmp("01", "01", 1000));

?>
--EXPECT--
strncasecmp(): Argument #3 ($length) must be greater than or equal to 0
int(0)
int(-3)
int(0)
int(0)
int(1)
int(0)
int(0)
