--TEST--
Turbofish arity: instance method, static method, nullsafe call all enforce arity
--FILE--
<?php
class C {
    public function m<T>($x) { return $x; }
    public static function s<A, B>($x) { return $x; }
}

$c = new C;

try { $c->m::<int, string>(1); }
catch (ArgumentCountError $e) { echo $e->getMessage(), "\n"; }

try { $c?->m::<int, string>(1); }
catch (ArgumentCountError $e) { echo $e->getMessage(), "\n"; }

try { C::s::<int>(1); }
catch (ArgumentCountError $e) { echo $e->getMessage(), "\n"; }
?>
--EXPECT--
Too many generic type arguments to C::m(), 2 passed and exactly 1 expected
Too many generic type arguments to C::m(), 2 passed and exactly 1 expected
Too few generic type arguments to C::s(), 1 passed and exactly 2 expected
