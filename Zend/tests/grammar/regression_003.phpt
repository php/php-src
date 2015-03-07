--TEST--
Test to ensure ::class is still reserved in obj scope
--FILE--
<?php

class Obj
{
    const CLASS = 'class';
}

--EXPECTF--
Fatal error: Cannot redefine class constant Obj::CLASS in %s on line 5
