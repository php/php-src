--TEST--
Test that the mixed return type is compatible with any kind of return value in strict mode
--FILE--
<?php
declare(strict_types=1);

function foo($a): mixed
{
    return $a;
}

foo(null);
foo(false);
foo(1);
foo("");
foo([]);
foo(new stdClass());

?>
--EXPECT--
