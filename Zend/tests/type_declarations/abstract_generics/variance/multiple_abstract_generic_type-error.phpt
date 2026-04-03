--TEST--
Multiple abstract generic type incorrect bound types in implementation
--FILE--
<?php

interface I<K, V> {
    public function set(K $key, V $value): void;
    public function get(K $key): V;
}

class C implements I<string, int> {
    public array $a = [];
    public function set(int $key, string $value): void {
        $this->a[$key] = $value . '!';
    }
    public function get(int $key): string {
        return $this->a[$key];
    }
}

?>
--EXPECTF--
Fatal error: Declaration of C::set(int $key, string $value): void must be compatible with I<K : string, V : int>::set(<K : string> $key, <V : int> $value): void in %s on line %d
