--TEST--
Test that a mixed return type can be overridden by any union return type
--FILE--
<?php

class Foo
{
    public function method(): mixed {}
}

class Bar1 extends Foo
{
    public function method(): bool|int|null {}
}

class Bar3 extends Foo
{
    public function method(): bool|int|float|string|array|object|null {}
}

class Bar4 extends Foo
{
    public function method(): stdClass|Foo {}
}

?>
--EXPECT--
