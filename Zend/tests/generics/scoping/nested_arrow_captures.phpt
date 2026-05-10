--TEST--
Scoping: nested arrow function captures enclosing type parameter
--FILE--
<?php
function f<T : int>(): Closure {
    return fn(T $y): T => $y;
}
$cl = f();
$r = new ReflectionFunction($cl);
echo $r->getParameters()[0]->getType()->getName(), "\n";
echo $r->getReturnType()->getName(), "\n";
?>
--EXPECT--
int
int
