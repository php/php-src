--TEST--
Test possible function naming regression on procedural scope
--FILE--
<?php

class Obj
{
    function echo(){} // valid
    function return(){} // valid
}

function echo(){} // not valid
--EXPECTF--
Parse error: syntax error, unexpected 'echo' (T_ECHO), expecting %s in %s on line 9
