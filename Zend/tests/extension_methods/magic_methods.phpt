--TEST--
Extension blocks may not declare magic methods
--FILE--
<?php
class C {}

extension C {
    public function __construct() {}
}
?>
--EXPECTF--
Fatal error: Extension blocks may not declare magic method __construct() in %s on line %d
