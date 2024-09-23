--TEST--
GMP operator overloading does not support non-stringable objects
--EXTENSIONS--
gmp
--FILE--
<?php

$num = gmp_init(42);
$o = new stdClass();

try {
    var_dump($num + $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num - $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num * $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num / $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num % $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num ** $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($num | $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num & $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num ^ $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num << $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($num >> $o);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
TypeError: Unsupported operand types: GMP + stdClass
TypeError: Unsupported operand types: GMP - stdClass
TypeError: Unsupported operand types: GMP * stdClass
TypeError: Unsupported operand types: GMP / stdClass
TypeError: Unsupported operand types: GMP % stdClass

Warning: Object of class stdClass could not be converted to int in %s on line %d
object(GMP)#4 (1) {
  ["num"]=>
  string(2) "42"
}
TypeError: Unsupported operand types: GMP | stdClass
TypeError: Unsupported operand types: GMP & stdClass
TypeError: Unsupported operand types: GMP ^ stdClass

Warning: Object of class stdClass could not be converted to int in %s on line %d
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "84"
}

Warning: Object of class stdClass could not be converted to int in %s on line %d
object(GMP)#3 (1) {
  ["num"]=>
  string(2) "21"
}
