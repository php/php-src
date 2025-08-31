--TEST--
Returning a reference from a function
--FILE--
<?php

function &returnByRef(&$arg1)
{
    return $arg1;
}

$a = 7;
$b =& returnByRef($a);
var_dump($b);
$a++;
var_dump($b);

?>
--EXPECT--
int(7)
int(8)
