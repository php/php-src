--TEST--
Intersection types in parameters
--FILE--
<?php

interface A {}
interface B {}

class Foo implements A, B {}
class Bar implements A {}

function foo(A&B|null $bar) {
    var_dump($bar);
}

foo(null);
foo(new Foo());

try {
    foo(new Bar());
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
NULL
object(Foo)#1 (0) {
}
foo(): Argument #1 ($bar) must be of type A&B|null, Bar given, called in %s on line %d
