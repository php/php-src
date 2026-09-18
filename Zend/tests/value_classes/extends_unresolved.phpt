--TEST--
A value class rejects a parent before autoloading
--FILE--
<?php
spl_autoload_register(function ($class) { echo "autoloaded\n"; });
value class Foo extends MissingParent {}
?>
--EXPECTF--
Fatal error: Value class Foo cannot extend another class in %s on line %d
