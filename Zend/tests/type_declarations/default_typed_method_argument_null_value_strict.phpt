--TEST--
Default null for non nullable method arguments using strict types
--FILE--
<?php

declare(strict_types=1);

final class Test
{
    private DateTimeImmutable $datetime;

    public function __construct(DateTimeImmutable $datetime = null)
    {
        $this->datetime = $datetime ?? new DateTimeImmutable();
    }
}

$test = new Test();

echo "Succeeded!";
?>
--EXPECT--
The operation should fail since $datetime is holding a different value than it's signature allows.
