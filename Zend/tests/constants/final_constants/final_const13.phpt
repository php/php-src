--TEST--
Bug GH-7757 (Multi-inherited final constant causes fatal error)
--FILE--
<?php
interface EntityInterface {
    final public const TEST = 'this';
}

interface KeyInterface extends EntityInterface {
}

interface StringableInterface extends EntityInterface {
}

class SomeTestClass implements KeyInterface, StringableInterface {
}
?>
--EXPECT--
