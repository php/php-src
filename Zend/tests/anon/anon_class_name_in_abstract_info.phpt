--TEST--
Anonymous class name is not truncated in the abstract method list
--FILE--
<?php
new class { abstract public $p { get; } };
?>
--EXPECTF--
Fatal error: Class class@anonymous%0%s must implement 1 abstract method (class@anonymous%0%s::$p::get) in %s on line %d
