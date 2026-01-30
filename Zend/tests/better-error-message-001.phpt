--TEST--
Better error message with mismatched inheritance between abstract class and interface
--FILE--
<?php
interface Foo
{
    public function getId(): int;
}

abstract class Baz
{
    public function getId(): mixed
    {
    }
}

class Lorem extends Baz implements Foo
{
}
?>
--EXPECTF--
Fatal error: While materializing Lorem: Declaration of Baz::getId(): mixed must be compatible with Foo::getId(): int in %sbetter-error-message-001.php on line %d
