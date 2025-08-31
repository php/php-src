--TEST--
Fiber interaction with custom fiber implementation 6
--EXTENSIONS--
zend_test
--FILE--
<?php
$test = new _ZendTestFiber(function (): void {
    $fiber = new Fiber(function (): void {
        var_dump(_ZendTestFiber::suspend(10)); // string(2) "10"
        Fiber::suspend(20);
        echo "unreachable\n"; // Fiber is not resumed.
    });
    var_dump($fiber->start()); // int(20)
});

$fiber = new Fiber(function (): void {
    var_dump(Fiber::suspend(1)); // string(1) "1"
    var_dump(Fiber::suspend(2)); // string(1) "2"
});

var_dump($test->start()); // int(10)

var_dump($fiber->start()); // int(1)
var_dump($fiber->resume('1')); // int(2)

var_dump($test->resume('10')); // NULL

var_dump($fiber->resume('2')); // NULL

?>
--EXPECT--
int(10)
int(1)
string(1) "1"
int(2)
string(2) "10"
int(20)
NULL
string(1) "2"
NULL
