--TEST--
Check that __invoke() works with named parameters
--FILE--
<?php

class Test {
    public function __invoke($a = 'a', $b = 'b') {
        echo "a: $a, b: $b\n";
    }
}

class Test2 {
    public function __invoke($a = 'a', $b = 'b', ...$rest) {
        echo "a: $a, b: $b\n";
        var_dump($rest);
    }
}

$test = new Test;
$test(b: 'B', a: 'A');
$test(b: 'B');
try {
    $test(b: 'B', c: 'C');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
echo "\n";

$test2 = new Test2;
$test2(b: 'B', a: 'A', c: 'C');
$test2(b: 'B', c: 'C');
echo "\n";

$test3 = function($a = 'a', $b = 'b') {
    echo "a: $a, b: $b\n";
};
$test3(b: 'B', a: 'A');
$test3(b: 'B');
try {
    $test3(b: 'B', c: 'C');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
a: A, b: B
a: a, b: B
Unknown named parameter $c

a: A, b: B
array(1) {
  ["c"]=>
  string(1) "C"
}
a: a, b: B
array(1) {
  ["c"]=>
  string(1) "C"
}

a: A, b: B
a: a, b: B
Unknown named parameter $c
