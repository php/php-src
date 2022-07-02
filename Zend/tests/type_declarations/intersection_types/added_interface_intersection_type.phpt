--TEST--
Added element of intersection type
--FILE--
<?php

interface X {}
interface Y {}
interface Z {}

class A implements X, Y, Z {}

class Collection {
    public X&Y $intersect;
}

function foo(): X&Y {
    return new A();
}

function bar(X&Y $o): void {
    var_dump($o);
}

$o = foo();
var_dump($o);

$c = new Collection();
$a = new A();

$c->intersect = $a;
echo 'OK', \PHP_EOL;
bar($a);
?>
--EXPECT--
object(A)#1 (0) {
}
OK
object(A)#3 (0) {
}
