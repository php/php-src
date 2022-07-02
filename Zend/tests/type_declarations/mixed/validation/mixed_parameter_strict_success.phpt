--TEST--
Test that the mixed parameter type accepts any kind of arguments in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo(mixed $a)
{
}

foo(null);
foo(false);
foo(1);
foo(3.14);
foo("");
foo([]);
foo(new stdClass());

?>
--EXPECT--
