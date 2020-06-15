--TEST--
Test possible constant naming regression on procedural scope
--FILE--
<?php

class Obj
{
    const return = 'yep';
}

const return = 'yep';
--EXPECT--

