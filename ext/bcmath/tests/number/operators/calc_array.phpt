--TEST--
BcMath\Number calc array by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);
$array = [1];

try {
    $num + $array;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num - $array;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num * $array;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num / $array;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % $array;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num ** $array;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Unsupported operand types: BcMath\Number + array
Unsupported operand types: BcMath\Number - array
Unsupported operand types: BcMath\Number * array
Unsupported operand types: BcMath\Number / array
Unsupported operand types: BcMath\Number % array
Unsupported operand types: BcMath\Number ** array
