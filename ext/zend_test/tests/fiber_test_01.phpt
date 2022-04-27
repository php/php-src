--TEST--
Fiber interaction with custom fiber implementation 1
--EXTENSIONS--
zend_test
--FILE--
<?php
$fiber = new Fiber(function (): int {
    $test = new _ZendTestFiber(function (): int {
        $value = Fiber::suspend(123);
        var_dump($value); // int(246)
        return $value;
    });
    var_dump($test->start()); // NULL
    return 1;
});
$value = $fiber->start();
var_dump($value); // int(123)
$value = $fiber->resume(2 * $value);
var_dump($value); // NULL
var_dump($fiber->getReturn()); // int(1)

?>
--EXPECT--
Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0

Warning: iterable type is now a compile time alias for array|Traversable, regenerate the argument info via the php-src gen_stub build script in Unknown on line 0
int(123)
int(246)
NULL
NULL
int(1)
