--TEST--
029: Name ambiguity (class name & import name)
--FILE--
<?php
use A\B as Foo;

class Foo {
}

new Foo();
?>
--EXPECTF--
Fatal error: Cannot redeclare class Foo (previously declared as local import) in %sns_029.php on line 4
