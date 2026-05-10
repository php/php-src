--TEST--
Scoping: nested closure has its own ReflectionFunction view
--FILE--
<?php
function f<T : object>(): Closure {
    return function (T $y): T { return $y; };
}
$cl = f();
$r = new ReflectionFunction($cl);
echo $r->getReturnType()->__toString(), "\n";
?>
--EXPECT--
object
