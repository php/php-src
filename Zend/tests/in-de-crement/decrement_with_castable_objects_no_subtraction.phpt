--TEST--
Decrementing objects which are castable to numeric types
--EXTENSIONS--
zend_test
--FILE--
<?php

$l = new LongCastableNoOperations(5);
$f = new FloatCastableNoOperations(15.8);
$nl = new NumericCastableNoOperations(52);
$nf = new NumericCastableNoOperations(58.3);

/* Check normal arithmetic */
try {
    var_dump($l - 1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump($f - 1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump($nl - 1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    var_dump($nf - 1);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

/* Decrement */
try {
    $l--;
    var_dump($l);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $f--;
    var_dump($f);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $nl--;
    var_dump($nl);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $nf--;
    var_dump($nf);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
TypeError: Unsupported operand types: LongCastableNoOperations - int
TypeError: Unsupported operand types: FloatCastableNoOperations - int
int(51)
float(57.3)
TypeError: Cannot decrement LongCastableNoOperations
TypeError: Cannot decrement FloatCastableNoOperations
int(51)
float(57.3)
