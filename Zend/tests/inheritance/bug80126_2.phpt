--TEST--
Bug #80126: Covariant return types failing compilation (variation 2)
--FILE--
<?php

interface I {
    public function method(): I;
}

abstract class A implements I {
    public function method(): I {
        return new static();
    }
}

class C extends A { }

interface I2 { }

class C2 extends C implements I2 {
    public function method(): C2 { }
}

?>
===DONE===
--EXPECT--
===DONE===
