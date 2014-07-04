--TEST--
Argument unpacking does not work with string keys (forward compatibility for named args)
--FILE--
<?php

set_error_handler(function($errno, $errstr) {
    var_dump($errstr);
});

var_dump(...[1, 2, "foo" => 3, 4]);
var_dump(...new ArrayIterator([1, 2, "foo" => 3, 4]));

?>
--EXPECTF--
string(36) "Cannot unpack array with string keys"
int(1)
int(2)
string(42) "Cannot unpack Traversable with string keys"
int(1)
int(2)
