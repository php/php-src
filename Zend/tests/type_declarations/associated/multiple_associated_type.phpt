--TEST--
Multiple associated types
--FILE--
<?php

interface I {
    type K;
    type V;
    public function set(K $key, V $value): void;
    public function get(K $key): V;
}

class C implements I {
    public function set(int $key, string $value): void {}
    public function get(int $key): string {}
}

?>
--EXPECTF--
Fatal error: Declaration of C::set(int $key, string $value): void must be compatible with I::set(K $key, V $value): void in %s on line %d
