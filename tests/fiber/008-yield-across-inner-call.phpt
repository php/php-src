--TEST--
tests Fiber::yield across inner call
--FILE--
<?php
$f = new Fiber(function () {
    array_map(function ($i) {
        Fiber::yield($i);
    }, [1]);
});

$f->resume();

?>
--EXPECTF--
Fatal error: Uncaught Error: Attempt to resume across internal call in %s008-yield-across-inner-call.php:%d
Stack trace:
#0 %s008-yield-across-inner-call.php(%d): Fiber::yield(%d)
#1 [internal function]: {closure}(%d)
#2 %s008-yield-across-inner-call.php(%d): array_map(Object(Closure), Array)
#3 (0): {closure}()
#4 {main}
  thrown in %s008-yield-across-inner-call.php on line %d
