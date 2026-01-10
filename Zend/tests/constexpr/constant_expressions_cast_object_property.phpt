--TEST--
Constant expressions with object cast in property
--FILE--
<?php
class X {
    public $foo = (object) [];
}
?>
--EXPECTF--
Fatal error: Object casts are not supported in this context in %s on line %d
