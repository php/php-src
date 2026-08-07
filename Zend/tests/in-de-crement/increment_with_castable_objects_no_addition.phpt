--TEST--
Incrementing objects which are castable to numeric types
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
    var_dump($l + 1);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($f + 1);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($nl + 1);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($nf + 1);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

/* Decrement */
try {
    $l++;
    var_dump($l);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $f++;
    var_dump($f);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $nl++;
    var_dump($nl);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $nf++;
    var_dump($nf);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
TypeError: Unsupported operand types: LongCastableNoOperations + int
TypeError: Unsupported operand types: FloatCastableNoOperations + int
int(53)
float(59.3)
TypeError: Cannot increment LongCastableNoOperations
TypeError: Cannot increment FloatCastableNoOperations
int(53)
float(59.3)
