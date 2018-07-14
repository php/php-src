--TEST--
iterable type#003 - Return types
--FILE--
<?php

function foo(): iterable {
	return [];
}
function bar(): iterable {
	return (function () { yield; })();
}
function stdClass(): iterable {
	return (object) [];
}

function baz(): iterable {
    return 1;
}

var_dump(foo());
var_dump(bar());
var_dump(stdClass());

try {
    baz();
} catch (Throwable $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
array(0) {
}
object(Generator)#2 (0) {
}
object(stdClass)#2 (0) {
}
Return value of baz() must be iterable, int returned
