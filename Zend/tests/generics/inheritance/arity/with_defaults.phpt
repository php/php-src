--TEST--
Inheritance arity: defaults extend the accepted range
--FILE--
<?php
class Pair<K, V = mixed> {}
interface Maybe<T = mixed> {}
trait Slot<T = int> {}

// Required-only arity
class P1 extends Pair<int> {}
// Including the optional
class P2 extends Pair<int, string> {}
// Trait with default — works without args
class T1 { use Slot; }

echo "OK\n";
?>
--EXPECT--
OK
