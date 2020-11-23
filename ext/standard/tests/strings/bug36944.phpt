--TEST--
Bug #36944 (strncmp & strncasecmp do not return false on negative string length)
--FILE--
<?php

try {
    var_dump(strncmp("test ", "e", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(strncmp("test ", "e", 10));
var_dump(strncmp("test ", "e", 0));

try {
    var_dump(strncasecmp("test ", "E", -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(strncasecmp("test ", "E", 10));
var_dump(strncasecmp("test ", "E", 0));

?>
--EXPECTF--
strncmp(): Argument #3 ($length) must be greater than or equal to 0
int(%d)
int(0)
strncasecmp(): Argument #3 ($length) must be greater than or equal to 0
int(%d)
int(0)
