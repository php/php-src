--TEST--
Catch exception thrown into fiber
--FILE--
<?php

$fiber = new Fiber(function () {
    try {
        Fiber::suspend('test');
    } catch (Exception $exception) {
        var_dump($exception->getMessage());
    }
});

$value = $fiber->start();
var_dump($value);

$fiber->throw(new Exception('test'));

?>
--EXPECT--
string(4) "test"
string(4) "test"
