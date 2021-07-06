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
int(1)
NULL
