--TEST--
__exists: directly callable as a method to disambiguate "set to null" from "missing"
--FILE--
<?php

/* The __exists method is part of the public API. It can be called directly
 * to ask "does this property logically exist?", independent of whether its
 * value is null. This is the gap that __isset cannot fill (since isset()
 * normalizes null to "not set"). */

class C {
    private array $store = ['nullProp' => null, 'intProp' => 5];
    public function __exists(string $n): bool {
        return array_key_exists($n, $this->store);
    }
    public function __get(string $n): mixed {
        return $this->store[$n] ?? null;
    }
}

$c = new C;

echo "1) Direct call: `nullProp` exists (set to null):\n";
var_dump($c->__exists('nullProp'));

echo "\n2) Direct call: `missing` does not exist:\n";
var_dump($c->__exists('missing'));

echo "\n3) But isset() collapses both to `false` (legacy isset semantics):\n";
var_dump(isset($c->nullProp));
var_dump(isset($c->missing));

?>
--EXPECT--
1) Direct call: `nullProp` exists (set to null):
bool(true)

2) Direct call: `missing` does not exist:
bool(false)

3) But isset() collapses both to `false` (legacy isset semantics):
bool(false)
bool(false)
