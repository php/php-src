--TEST--
__exists: parameter follows variance rules (untyped allowed; widened allowed in subclass)
--FILE--
<?php

class Untyped {
    public function __exists($n): bool {
        echo "  __exists("; var_export($n); echo ")\n";
        return $n === 'present';
    }
    public function __get($n): mixed {
        return "value-of-$n";
    }
}

echo "1) Untyped parameter is allowed (mirrors __isset BC behaviour):\n";
$o = new Untyped;
var_dump($o->present ?? 'fb');
var_dump($o->absent  ?? 'fb');

class Parent_ {
    public function __exists(string $n): bool { return false; }
    public function __get(string $n): mixed { return "p-$n"; }
}
class Child extends Parent_ {
    /* Contravariant widening: subclass accepts a wider input set than the parent. */
    public function __exists(string|int $n): bool {
        echo "  Child::__exists("; var_export($n); echo ")\n";
        return $n === 'present';
    }
}

echo "\n2) Subclass may widen parameter type (contravariance):\n";
$c = new Child;
var_dump($c->present ?? 'fb');
var_dump($c->absent  ?? 'fb');

?>
--EXPECT--
1) Untyped parameter is allowed (mirrors __isset BC behaviour):
  __exists('present')
string(16) "value-of-present"
  __exists('absent')
string(2) "fb"

2) Subclass may widen parameter type (contravariance):
  Child::__exists('present')
string(9) "p-present"
  Child::__exists('absent')
string(2) "fb"
