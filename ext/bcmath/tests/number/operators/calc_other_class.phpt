--TEST--
BcMath\Number calc other class by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);
$other = new stdClass();

try {
    $num + $other;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num - $other;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num * $other;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num / $other;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % $other;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num ** $other;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Unsupported operand types: BcMath\Number + stdClass
Unsupported operand types: BcMath\Number - stdClass
Unsupported operand types: BcMath\Number * stdClass
Unsupported operand types: BcMath\Number / stdClass
Unsupported operand types: BcMath\Number % stdClass
Unsupported operand types: BcMath\Number ** stdClass
