--TEST--
Cannot have explicitly abstract property in interface
--FILE--
<?php

interface I {
    abstract public $prop { get; set; }
}

?>
--EXPECTF--
Fatal error: Property in interface cannot be explicitly abstract. All interface members are implicitly abstract in %s on line %d
