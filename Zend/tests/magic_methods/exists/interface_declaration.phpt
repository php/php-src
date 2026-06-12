--TEST--
__exists: allowed in interfaces, no special engine treatment
--FILE--
<?php

/* __exists may be declared on an interface, exactly like __isset and __get.
 * The engine attaches no special meaning to the interface declaration; it
 * only consults the implementing class's method. */

interface I {
    public function __exists(string $n): bool;
}

class C implements I {
    public function __exists(string $n): bool {
        echo "  C::__exists($n)\n";
        return $n === 'present';
    }
    public function __get(string $n): mixed {
        return "value-of-$n";
    }
}

$c = new C;
var_dump($c->present ?? 'fb');
var_dump($c->absent  ?? 'fb');
var_dump($c instanceof I);

?>
--EXPECT--
  C::__exists(present)
string(16) "value-of-present"
  C::__exists(absent)
string(2) "fb"
bool(true)
