--TEST--
Test that the number parameter type accepts integer and float arguments in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo(number $a)
{
    var_dump($a);
}

foo(1);
foo(3.14);

?>
--EXPECT--
int(1)
float(3.14)
