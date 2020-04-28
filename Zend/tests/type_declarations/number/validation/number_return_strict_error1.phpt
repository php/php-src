--TEST--
Test that the number return type is not compatible with a void return value in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo($value): number
{
}

?>
--EXPECT--

