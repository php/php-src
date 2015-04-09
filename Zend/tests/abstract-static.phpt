--TEST--
Test for abstract static classes
--FILE--
<?php
abstract class TestClass
{
    abstract static public function getName();
}
?>
===DONE===
--EXPECTF--

Strict Standards: Static function TestClass::getName() should not be abstract in %sabstract-static.php on line %d
===DONE===
