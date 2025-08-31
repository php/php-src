--TEST--
Test that the mixed parameter type accepts any kind of arguments in weak mode
--FILE--
<?php

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
