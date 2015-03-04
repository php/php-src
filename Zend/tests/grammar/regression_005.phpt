--TEST--
Test possible constant naming regression on procedural scope
--FILE--
<?php

class Obj
{
    const return = 'yep';
}

const return = 'nope';

--EXPECTF--
Parse error: syntax error, unexpected 'return' (T_RETURN), expecting identifier (T_STRING) in %s on line 8
