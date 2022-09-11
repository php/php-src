--TEST--
Test possible function naming regression on procedural scope now allowed
--FILE--
<?php

class Obj
{
    function echo(){} // valid
    function return(){} // valid
}

function echo(){print("test\n");} // valid
\echo();
namespace\echo();
?>
--EXPECT--
test
test
