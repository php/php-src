--TEST--
Turbofish arity: new <Class>::<...> enforces arity against class generic parameters
--FILE--
<?php
class Box<T> {
    public function __construct(public int $v) {}
}
class Pair<K, V = mixed> {}
class Plain {}

try { new Box::<int, string>(1); }
catch (ArgumentCountError $e) { echo $e->getMessage(), "\n"; }

try { new Plain::<int>(); }
catch (ArgumentCountError $e) { echo $e->getMessage(), "\n"; }

// Pair: K required, V has default - so 1 or 2 args is OK, 0 or 3 is not
$p = new Pair::<int>();
echo get_class($p), "\n";

try { new Pair::<int, string, float>(); }
catch (ArgumentCountError $e) { echo $e->getMessage(), "\n"; }
?>
--EXPECT--
Too many generic type arguments to new Box, 2 passed and exactly 1 expected
Too many generic type arguments to new Plain, 1 passed and exactly 0 expected
Pair
Too many generic type arguments to new Pair, 3 passed and at most 2 expected
