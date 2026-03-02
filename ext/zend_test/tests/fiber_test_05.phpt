--TEST--
Fiber interaction with custom fiber implementation 5
--EXTENSIONS--
zend_test
--FILE--
<?php
$test = new _ZendTestFiber(function (): void {
    $fiber = new Fiber(function (): int {
        $value = Fiber::suspend(1);
        var_dump($value); // int(2)
        $value = _ZendTestFiber::suspend(3);
        var_dump($value); // int(6)
        $value = Fiber::suspend(4);
        var_dump($value); // int(8)
        return 2 * $value;
    });
    $value = $fiber->start();
    var_dump($value); // int(1)
    $value = $fiber->resume(2 * $value);
    var_dump($value); // int(4)
    $value = $fiber->resume(2 * $value);
    var_dump($value); // NULL
    var_dump($fiber->getReturn()); // int(16)
});
$value = $test->start();
var_dump($value); // int(3)
$value = $test->resume(2 * $value);

?>
--EXPECT--
int(1)
int(2)
int(3)
int(6)
int(4)
int(8)
NULL
int(16)
