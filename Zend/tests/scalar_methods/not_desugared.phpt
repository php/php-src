--TEST--
Scalar methods: only static-string receivers desugar; vars/objects/nullsafe unaffected
--FILE--
<?php
// An untyped variable holding a string is NOT a compile-time static string,
// so its method call is left untouched and fails as before.
try {
    $s = "hi";
    $s->trim();
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}

// A genuine object's method call works normally.
class C {
    public function trim() { return "object-trim"; }
}
$o = new C();
var_dump($o->trim());

// Nullsafe on a string literal is deliberately excluded from desugaring and
// behaves exactly as before.
try {
    var_dump("x"?->trim());
} catch (\Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Call to a member function trim() on string
string(11) "object-trim"
Call to a member function trim() on string
