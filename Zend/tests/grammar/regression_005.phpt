--TEST--
Test possible constant naming regression on procedural scope now allowed
--FILE--
<?php

class Obj
{
    const return = 'yep';
}

const return = 'nope';
var_dump(\return);
var_dump(Obj::return);
?>
--EXPECT--
string(4) "nope"
string(3) "yep"
