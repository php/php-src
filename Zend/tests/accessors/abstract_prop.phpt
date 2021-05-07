--TEST--
Whole property can be marked abstract, but must use accessors
--FILE--
<?php

abstract class A {
    abstract public $prop { get; set; }
}

class B extends A {
    public $prop { get; set; }
}

?>
===DONE===
--EXPECT--
===DONE===
