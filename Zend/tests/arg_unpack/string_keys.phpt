--TEST--
Argument unpacking does not work with string keys (forward compatibility for named args)
--FILE--
<?php

set_error_handler(function($errno, $errstr) {
    var_dump($errstr);
});

try {
	var_dump(...[1, 2, "foo" => 3, 4]);
} catch (Error $ex) {
	var_dump($ex->getMessage());
}
try {
	var_dump(...new ArrayIterator([1, 2, "foo" => 3, 4]));
} catch (Error $ex) {
	var_dump($ex->getMessage());
}

?>
--EXPECT--
string(36) "Cannot unpack array with string keys"
string(42) "Cannot unpack Traversable with string keys"
