--TEST--
Symmetric coroutine does not leak prior context
--EXTENSIONS--
zend_test
--FILE--
<?php

$fiber = new _ZendTestFiber(function (): int {
    return 1;
});

$fiber->pipeTo(function (int $result): void {
    var_dump($result);
});

var_dump($fiber->start());

?>
--EXPECT--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
int(1)
NULL
