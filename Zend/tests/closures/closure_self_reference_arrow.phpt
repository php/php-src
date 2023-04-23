--TEST--
Testing Closure self-reference functionality: Arrow functions
--FILE--
<?php

$factorial = fn(int $num) as $fn : int => $num > 1 ? $num * $fn($num - 1) : $num;

echo "5! = ", $factorial(5), PHP_EOL;


$fn = fn() as $a => fn(int $b) as $a => $a;

echo new ReflectionParameter($fn()(1), 'b'), PHP_EOL;


$c = 123;
$fn = fn() as $c => fn() => $c;

echo get_class($fn()()), PHP_EOL;

?>
--EXPECTF--
5! = 120
Parameter #0 [ <required> int $b ]
Closure
