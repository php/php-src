--TEST--
Test throwing into fiber
--FILE--
<?php

$fiber = new Fiber(function (): void {
    Fiber::suspend('test');
});

$value = $fiber->start();
var_dump($value);

$fiber->throw(new Exception('test'));

?>
--EXPECTF--
string(4) "test"

Fatal error: Uncaught Exception: test in %sthrow.php:%d
Stack trace:
#0 {main}
  thrown in %sthrow.php on line %d
