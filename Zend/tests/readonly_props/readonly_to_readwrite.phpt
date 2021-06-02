--TEST--
Can replace readonly with readwrite
--FILE--
<?php

class A {
    public readonly int $prop;
}
class B extends A {
    public int $prop;
}

?>
===DONE===
--EXPECT--
===DONE===
