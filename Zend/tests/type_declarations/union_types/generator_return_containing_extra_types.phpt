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
--EXPECTF--
object(Generator)#%d (1) {
  ["function"]=>
  string(7) "C::test"
}
