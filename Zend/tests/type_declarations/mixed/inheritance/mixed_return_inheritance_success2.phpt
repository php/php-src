--TEST--
Test that a mixed return type can be overridden by any single (and nullable) type except void
--FILE--
<?php

class Foo
{
    public function method(): mixed {}
}

class Bar1 extends Foo
{
    public function method(): bool {}
}

class Bar2 extends Foo
{
    public function method(): int {}
}

class Bar3 extends Foo
{
    public function method(): float {}
}

class Bar4 extends Foo
{
    public function method(): string {}
}

class Bar5 extends Foo
{
    public function method(): array {}
}

class Bar6 extends Foo
{
    public function method(): object {}
}

class Bar7 extends Foo
{
    public function method(): stdClass {}
}

class Bar8 extends Foo
{
    public function method(): ?int {}
}

class Bar9 extends Foo
{
    public function method(): ?stdClass {}
}

?>
--EXPECT--
