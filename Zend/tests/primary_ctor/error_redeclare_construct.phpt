--TEST--
Primary constructors: cannot also declare an explicit __construct()
--FILE--
<?php
class Bad(public int $x) {
    public function __construct(public int $x) {}
}
?>
--EXPECTF--
Fatal error: Cannot redeclare Bad::__construct() in %s on line %d
