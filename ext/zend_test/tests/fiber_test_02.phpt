--TEST--
Fiber interaction with custom fiber implementation 2
--EXTENSIONS--
zend_test
--FILE--
<?php
$test = new _ZendTestFiber(function (): void {
    $fiber = new Fiber(function (): int {
        $value = _ZendTestFiber::suspend(1);
        var_dump($value); // int(2)
        return $value;
    });
    $fiber->start();
});
var_dump($test->start()); // int(1)
var_dump($test->resume(2)); // NULL

?>
--EXPECT--
int(1)
int(2)
NULL
