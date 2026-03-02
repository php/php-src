--TEST--
public(set) is allowed on non-readonly
--FILE--
<?php

class A {
    public public(set) int $prop;
}

?>
===DONE===
--EXPECT--
===DONE===
