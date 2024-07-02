--TEST--
Abstract property without hook is illegal
--FILE--
<?php

class C {
    abstract public $prop;
}

?>
--EXPECTF--
Fatal error: Only hooked properties may be declared abstract in %s on line %d
