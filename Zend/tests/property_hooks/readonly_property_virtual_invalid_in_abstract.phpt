--TEST--
Property hook cannot be both abstract and readonly
--FILE--
<?php
abstract class Test {
    abstract public readonly int $prop { get; }
}
?>
--EXPECTF--
Fatal error: Abstract hooked properties cannot be declared readonly in %s on line %d