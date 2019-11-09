--TEST--
Generator return value has to have Traversable-ish, but may also have extra types
--FILE--
<?php

interface I {
    public function test(): iterable|false;
}

class C implements I {
    public function test(): iterable|false {
        yield;
    }
}

var_dump((new C)->test());

?>
--EXPECT--
object(Generator)#2 (0) {
}
