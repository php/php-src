--TEST--
Scoping: nested closure captures enclosing function's type parameter
--FILE--
<?php
function f<T : object>(T $x): Closure {
    return function (T $y): T { return $y; };
}
class Foo {}
$cl = f(new Foo);
$r = new ReflectionFunction($cl);
echo $r->getParameters()[0]->getType()->getName(), "\n";
echo $r->getReturnType()->getName(), "\n";
?>
--EXPECT--
object
object
