--TEST--
Test for abstract static classes
--FILE--
<?php
abstract class Test
{
    abstract static public function getName();
}
?>
DONE
--EXPECTF--

Fatal error: Static function Test::getName() cannot be abstract in %sabstract-static.php on line %d
