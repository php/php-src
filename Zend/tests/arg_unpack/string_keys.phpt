--TEST--
Argument unpacking does not work with string keys (forward compatibility for named args)
--FILE--
<?php

set_error_handler(function($errno, $errstr) {
    var_dump($errstr);
});

try {
    var_dump(...new ArrayIterator([1, 2, "foo" => 3, 4]));
} catch (Error $ex) {
    var_dump($ex->getMessage());
}

?>
--EXPECT--
string(68) "Cannot use positional argument after named argument during unpacking"
