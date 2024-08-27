--TEST--
BcMath\Number calc undefined var by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num + $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num - $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num * $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num / $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num ** $undef;
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
Warning: Undefined variable $undef in %s

Warning: Undefined variable $undef in %s

Warning: Undefined variable $undef in %s

Warning: Undefined variable $undef in %s
Division by zero

Warning: Undefined variable $undef in %s
Modulo by zero

Warning: Undefined variable $undef in %s
