--TEST--
Explicit hooked property satisfies abstract property
--FILE--
<?php

abstract class A {
    abstract public $prop { get; set; }
}

class B extends A {
    public $prop { get {} set {} }
}

?>
===DONE===
--EXPECT--
===DONE===
