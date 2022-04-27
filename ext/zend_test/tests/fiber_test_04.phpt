--TEST--
Fiber interaction with custom fiber implementation 4
--EXTENSIONS--
zend_test
--FILE--
<?php
$fiber = new Fiber(function (): int {
    $test = new _ZendTestFiber(function (): void {
        $value = Fiber::suspend(1);
        var_dump($value); // int(2)
        $value = _ZendTestFiber::suspend(3);
        var_dump($value); // int(6)
        $value = Fiber::suspend(4);
        var_dump($value); // int(8)
        _ZendTestFiber::suspend(5);
        echo "unreachable\n"; // Test fiber is not resumed.
    });
    $value = $test->start();
    var_dump($value); // int(3)
    var_dump($test->resume(2 * $value)); // int(5)
    return -1;
});
$value = $fiber->start();
var_dump($value); // int(1)
$value = $fiber->resume(2 * $value);
var_dump($value); // int(4)
$value = $fiber->resume(2 * $value);
var_dump($value); // NULL
var_dump($fiber->getReturn()); // int(-1)

?>
--EXPECT--
int(1)
int(2)
int(3)
int(6)
int(4)
int(8)
int(5)
NULL
int(-1)
