--TEST--
Test that the number return type is compatible with any kind of numeric type in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo($a): number
{
    return $a;
}

foo(1);
foo(3.14);

?>
--EXPECT--
