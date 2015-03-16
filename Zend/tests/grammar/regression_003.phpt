--TEST--
Test to ensure ::class is reserved in obj scope
--FILE--
<?php

class Obj
{
    const CLASS = 'class';
}

--EXPECTF--
Fatal error: Cannot redefine class constant Obj::CLASS as it is reserved in %s on line 5
