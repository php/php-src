--TEST--
strncmp() tests
--FILE--
<?php

var_dump(strncmp("", "", 100));
try {
    var_dump(strncmp("aef", "dfsgbdf", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(strncmp("fghjkl", "qwer", 0));
var_dump(strncmp("qwerty", "qwerty123", 6));
var_dump(strncmp("qwerty", "qwerty123", 7));

?>
--EXPECT--
int(0)
strncmp(): Argument #3 ($length) must be greater than or equal to 0
int(0)
int(0)
int(-1)
