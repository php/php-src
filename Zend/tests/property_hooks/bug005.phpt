--TEST--
Parent hook call restriction may fail due to mangled name
--FILE--
<?php

class B {
    protected mixed $x;
}

class C extends B {
    protected mixed $x {
        set {
            parent::$x::set(1);
        }
    }
}

?>
===DONE===
--EXPECT--
===DONE===
