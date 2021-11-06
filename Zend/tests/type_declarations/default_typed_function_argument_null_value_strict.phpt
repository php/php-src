--TEST--
Default null for non nullable function arguments using strict types
--FILE--
<?php

declare(strict_types=1);

function anotherTest(DateTimeImmutable $datetime = null): DateTimeImmutable
{
    return $datetime ?? new DateTimeImmutable();
}

$test = anotherTest();

echo "Succeeded!";
?>
--EXPECT--
The operation should fail since $datetime is holding a different value than it's signature allows.
