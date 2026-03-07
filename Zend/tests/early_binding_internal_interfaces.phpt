--TEST--
Early binding should not be prevented by internal interfaces
--FILE--
<?php

// Test 1: Implicit Stringable from __toString() should not block hoisting
class B1 extends A1 {}
class A1 {
    public function __toString(): string { return 'A1'; }
}
$b1 = new B1();
echo "Test 1 (implicit Stringable): " . $b1 . "\n";
var_dump($b1 instanceof Stringable);

// Test 2: Explicit implements Stringable should not block hoisting
class B2 extends A2 {}
class A2 implements Stringable {
    public function __toString(): string { return 'A2'; }
}
$b2 = new B2();
echo "Test 2 (explicit Stringable): " . $b2 . "\n";
var_dump($b2 instanceof Stringable);

// Test 3: Countable should not block hoisting
class B3 extends A3 {}
class A3 implements Countable {
    public function count(): int { return 42; }
}
$b3 = new B3();
echo "Test 3 (Countable): " . count($b3) . "\n";
var_dump($b3 instanceof Countable);

// Test 4: ArrayAccess should not block hoisting
class B4 extends A4 {}
class A4 implements ArrayAccess {
    public function offsetExists(mixed $offset): bool { return $offset === 'x'; }
    public function offsetGet(mixed $offset): mixed { return 'val'; }
    public function offsetSet(mixed $offset, mixed $value): void {}
    public function offsetUnset(mixed $offset): void {}
}
$b4 = new B4();
echo "Test 4 (ArrayAccess): " . $b4['x'] . "\n";
var_dump($b4 instanceof ArrayAccess);

// Test 5: IteratorAggregate should not block hoisting
class B5 extends A5 {}
class A5 implements IteratorAggregate {
    public function getIterator(): Traversable { return new ArrayIterator([1, 2]); }
}
$b5 = new B5();
echo "Test 5 (IteratorAggregate):";
foreach ($b5 as $v) echo " $v";
echo "\n";
var_dump($b5 instanceof IteratorAggregate);

// Test 6: Multiple internal interfaces combined
class B6 extends A6 {}
class A6 implements Stringable, Countable {
    public function __toString(): string { return 'A6'; }
    public function count(): int { return 6; }
}
$b6 = new B6();
echo "Test 6 (Stringable+Countable): " . $b6 . " count=" . count($b6) . "\n";
var_dump($b6 instanceof Stringable && $b6 instanceof Countable);

// Test 7: Child with __toString() extending abstract parent with explicit Stringable
class B7 extends A7 {
    public function __toString(): string { return 'B7'; }
}
abstract class A7 implements Stringable {}
$b7 = new B7();
echo "Test 7 (child __toString, abstract parent Stringable): " . $b7 . "\n";
var_dump($b7 instanceof Stringable);

// Test 8: Both parent and child have __toString()
class B8 extends A8 {
    public function __toString(): string { return 'B8'; }
}
class A8 {
    public function __toString(): string { return 'A8'; }
}
$b8 = new B8();
echo "Test 8 (both have __toString): " . $b8 . "\n";

// Test 9: String casting works correctly through inheritance
class B9 extends A9 {}
class A9 {
    public function __toString(): string { return 'A9_value'; }
}
$cast = (string) new B9();
echo "Test 9 (casting): $cast\n";
var_dump($cast === 'A9_value');

?>
--EXPECT--
Test 1 (implicit Stringable): A1
bool(true)
Test 2 (explicit Stringable): A2
bool(true)
Test 3 (Countable): 42
bool(true)
Test 4 (ArrayAccess): val
bool(true)
Test 5 (IteratorAggregate): 1 2
bool(true)
Test 6 (Stringable+Countable): A6 count=6
bool(true)
Test 7 (child __toString, abstract parent Stringable): B7
bool(true)
Test 8 (both have __toString): B8
Test 9 (casting): A9_value
bool(true)
