--TEST--
Test possible constant naming regression on procedural scope
--FILE--
<?php

class Obj
{
    const return = 'yep';
}

const return = 'nope';
?>
--EXPECTF--
Parse error: syntax error, unexpected token "return", expecting identifier in %s on line %d
