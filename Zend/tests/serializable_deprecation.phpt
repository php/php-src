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
Deprecated: The Serializable interface is deprecated. If you need to retain the Serializable interface for cross-version compatibility, you can suppress this warning by implementing __serialize() and __unserialize() in addition, which will take precedence over Serializable in PHP versions that support them in %s on line 6
