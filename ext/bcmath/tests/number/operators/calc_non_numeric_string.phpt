--TEST--
BcMath\Number calc non-numeric string by operator
--EXTENSIONS--
bcmath
--FILE--
<?php
$num = new BcMath\Number(100);

try {
    $num + 'a';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num - 'a';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num * 'a';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num / 'a';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num % 'a';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num ** 'a';
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
Number is not well-formed
Number is not well-formed
Number is not well-formed
Number is not well-formed
Number is not well-formed
Number is not well-formed
