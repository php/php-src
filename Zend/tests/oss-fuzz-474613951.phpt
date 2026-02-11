--TEST--
OSS-Fuzz #474613951: Leaked parent property default value
--FILE--
<?php

class A {
    public $prop = C { get => $this->prop; }
}

class B extends A {
    public $prop { get => 42; }
}

?>
===DONE===
--EXPECT--
===DONE===
