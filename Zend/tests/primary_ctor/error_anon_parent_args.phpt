--TEST--
Primary constructors: anonymous classes cannot forward arguments to the parent
--FILE--
<?php
class Base { public function __construct(public int $id) {} }
$o = new class extends Base(5) {};
?>
--EXPECTF--
Fatal error: Cannot pass arguments to the parent constructor without a primary constructor in %s on line %d
