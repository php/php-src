--TEST--
Arguments to fiber callback
--FILE--
<?php

$fiber = new Fiber(function (int $x): int {
    return $x + Fiber::suspend($x);
});

$x = $fiber->start(1);
$fiber->resume(0);
var_dump($fiber->getReturn());

$fiber = new Fiber(function (int $x): int {
    return $x + Fiber::suspend($x);
});

$fiber->start('test');

?>
--EXPECTF--
int(1)

Fatal error: Uncaught TypeError: {closure:%s:%d}(): Argument #1 ($x) must be of type int, string given in %s:%d
Stack trace:
#0 [internal function]: {closure:%s:%d}('test')
#1 %sstart-arguments.php(%d): Fiber->start('test')
#2 {main}
  thrown in %sstart-arguments.php on line %d
