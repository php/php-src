--TEST--
Property hooks in interfaces cannot be explicitly abstract
--FILE--
<?php

interface I {
    public abstract $prop { get; }
}

?>
--EXPECTF--
Fatal error: Property in interface cannot be explicitly abstract. All interface members are implicitly abstract in %s on line %d
