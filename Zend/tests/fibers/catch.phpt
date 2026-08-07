--TEST--
Catch exception thrown into fiber
--FILE--
<?php

$fiber = new Fiber(function () {
    try {
        Fiber::suspend('test');
    } catch (Exception $exception) {
        echo $exception::class, ': ', $exception->getMessage(), "\n";
    }
});

$value = $fiber->start();
var_dump($value);

$fiber->throw(new Exception('test'));

?>
--EXPECT--
string(4) "test"
Exception: test
