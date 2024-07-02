--TEST--
Relaxed type compatibility for read-only and write-only properties
--FILE--
<?php

class A {
    public int|float $a { get { return 42.0; } }
    public int $b { set { } }
}
class B extends A {
    public int $a { get { return 42; } }
    public int|float $b { set { } }
}

?>
===DONE===
--EXPECT--
===DONE===
