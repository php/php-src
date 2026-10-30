--TEST--
BcMath\Number calc undefined var by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num + null;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num - null;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num * null;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num / null;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % null;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num ** null;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Unsupported operand types: BcMath\Number + null
Unsupported operand types: BcMath\Number - null
Unsupported operand types: BcMath\Number * null
Unsupported operand types: BcMath\Number / null
Unsupported operand types: BcMath\Number % null
Unsupported operand types: BcMath\Number ** null
