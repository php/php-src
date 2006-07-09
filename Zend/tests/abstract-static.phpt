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

Fatal error: Static function TestClass::getName() cannot be abstract in %sabstract-static.php on line %d
