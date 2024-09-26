--TEST--
Better error message with mismatched materialisation of trait
--FILE--
<?php
interface Foo
{
    public function getId(): int;
}

class Baz implements Foo
{
    use FooTrait;
}

trait FooTrait
{
    public function getId(): mixed
    {
    }
}
?>
--EXPECTF--
Fatal error: Declaration of Baz::getId(): mixed (from FooTrait) must be compatible with Foo::getId(): int in %sbetter-error-message-002.php on line %d
