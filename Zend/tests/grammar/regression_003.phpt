--TEST--
Test to ensure ::class is still reserved in obj scope
--FILE--
<?php

class Obj
{
    const CLASS = 'class';
}

?>
--EXPECTF--
Fatal error: A class constant must not be called 'class'; it is reserved for class name fetching in %s on line %d
