<?php
declare(strict_types=1);

final class RegularBookingId
{
    public function __construct(public readonly string $value) {}
    public function toString(): string { return $this->value; }
}

readonly class ReadonlyBookingId
{
    public function __construct(public string $value) {}
    public function toString(): string { return $this->value; }
}

value class ValueBookingId
{
    public function __construct(public string $value) {}
    public function toString(): string { return $this->value; }
}
