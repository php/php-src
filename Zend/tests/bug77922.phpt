--TEST--
Bug #77922: Double release of doc comment on inherited shadow property
--FILE--
<?php

class A {
    /** Foo */
    private $prop;
}

class B extends A {
}

class C extends B {
}

?>
===DONE===
--EXPECT--
===DONE===
