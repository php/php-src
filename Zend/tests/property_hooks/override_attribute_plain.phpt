--TEST--
Override attribute is satisfied by plain property
--FILE--
<?php

class A {
    public $prop;
}

class B extends A {
    public $prop {
        #[Override]
        get => parent::$prop::get();
    }
}

?>
===DONE===
--EXPECT--
===DONE===
