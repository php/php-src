--TEST--
__exists: inheritance, child __exists overrides parent __isset
--FILE--
<?php

class Parent_ {
    public function __isset(string $n): bool {
        throw new Exception("Parent::__isset must not be called when child defines __exists");
    }
    public function __get(string $n): mixed {
        echo "  Parent::__get($n)\n";
        return "g($n)";
    }
}

class Child extends Parent_ {
    public function __exists(string $n): bool {
        echo "  Child::__exists($n)\n";
        return $n === 'present';
    }
}

echo "1) `??` on child: uses Child::__exists, ignores Parent::__isset:\n";
$c = new Child;
var_dump($c->present ?? 'fb');
var_dump($c->absent ?? 'fb');

echo "\n2) isset() on child: uses Child::__exists + Parent::__get:\n";
$c = new Child;
var_dump(isset($c->present));

?>
--EXPECT--
1) `??` on child: uses Child::__exists, ignores Parent::__isset:
  Child::__exists(present)
  Parent::__get(present)
string(10) "g(present)"
  Child::__exists(absent)
string(2) "fb"

2) isset() on child: uses Child::__exists + Parent::__get:
  Child::__exists(present)
  Parent::__get(present)
bool(true)
