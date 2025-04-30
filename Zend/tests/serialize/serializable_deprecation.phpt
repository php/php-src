--TEST--
Serializable deprecation
--FILE--
<?php

interface I extends Serializable {}
abstract class A implements Serializable {}

class C extends A implements I {
    public function serialize(): string {}
    public function unserialize(string $data) {}
}

class D extends A implements I {
    public function serialize(): string {}
    public function unserialize(string $data) {}
    public function __serialize(): array {}
    public function __unserialize(array $data) {}
}

?>
--EXPECTF--
Deprecated: %s implements the Serializable interface, which is deprecated. Implement __serialize() and __unserialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d
