--TEST--
Native shift left with invalid op2
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    $n = gmp_init("6");
    var_dump($n << "nonsense");
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    $n = gmp_init("6");
    var_dump($n << new stdClass());
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    $n = gmp_init("6");
    var_dump($n << STDERR);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
try {
    $n = gmp_init("6");
    var_dump($n << []);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}


echo "Done\n";
?>
--EXPECT--
TypeError: Unsupported operand types: GMP << string
TypeError: Unsupported operand types: GMP << stdClass
TypeError: Unsupported operand types: GMP << resource
TypeError: Unsupported operand types: GMP << array
Done
