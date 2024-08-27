--TEST--
BcMath\Number mod by zero by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num % 0;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % '0';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % (new BcMath\Number(0));
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

$zero = new BcMath\Number(0);

try {
    100 % $zero;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    '100' % $zero;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % $zero;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Modulo by zero
Modulo by zero
Modulo by zero
Modulo by zero
Modulo by zero
Modulo by zero
