--TEST--
Test to ensure ::class is still reserved in obj scope
--FILE--
<?php

class Obj
{
    const CLASS = 'class';
}

--EXPECTF--
Parse error: syntax error, unexpected 'CLASS' (T_CLASS) in %s on line 5
