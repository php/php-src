--TEST--
iterable type with object should be allowed in variance checks
--FILE--
<?php

class A {
    public object|iterable $x;
    public object|array $y;
}
class B extends A {
    public object|array $x;
    public object|iterable $y;
}

?>
===DONE===
--EXPECT--
===DONE===
