--TEST--
Generic closures and arrow functions
--FILE--
<?php
declare(strict_types=1);

// Generic closure
$identity = function<T>(T $value): T {
    return $value;
};

echo $identity(42) . "\n";       // 42
echo $identity("hello") . "\n";  // hello

// Generic arrow function
$wrap = fn<T>(T $x): array => [$x];
var_dump($wrap(42));       // array(1) { [0]=> int(42) }
var_dump($wrap("hello"));  // array(1) { [0]=> string(5) "hello" }

// Generic closure with constraint
$count = function<T: Countable>(T $item): int {
    return count($item);
};

echo $count(new ArrayObject([1, 2, 3])) . "\n"; // 3

// Multiple type params
$pair = fn<A, B>(A $a, B $b): array => [$a, $b];
var_dump($pair(1, "two"));  // array(2) { [0]=> int(1) [1]=> string(3) "two" }

// Reflection on generic closures
$rf = new ReflectionFunction($identity);
var_dump($rf->isGeneric()); // true
$params = $rf->getGenericParameters();
echo "identity generic params: " . count($params) . "\n";
echo "  T: " . $params[0]->getName() . "\n";

// Reflection on generic arrow function
$rf2 = new ReflectionFunction($wrap);
var_dump($rf2->isGeneric()); // true

// Non-generic closure for comparison
$plain = function(int $x): int { return $x * 2; };
$rf3 = new ReflectionFunction($plain);
var_dump($rf3->isGeneric()); // false

echo "OK\n";
?>
--EXPECT--
42
hello
array(1) {
  [0]=>
  int(42)
}
array(1) {
  [0]=>
  string(5) "hello"
}
3
array(2) {
  [0]=>
  int(1)
  [1]=>
  string(3) "two"
}
bool(true)
identity generic params: 1
  T: T
bool(true)
bool(false)
OK
