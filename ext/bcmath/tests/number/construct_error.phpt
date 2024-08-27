--TEST--
BcMath\Number construct error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    $num = new BcMath\Number();
    var_dump($num);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num = new BcMath\Number(1, 1);
    var_dump($num);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    $num = new BcMath\Number('a');
    var_dump($num);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
BcMath\Number::__construct() expects exactly 1 argument, 0 given
BcMath\Number::__construct() expects exactly 1 argument, 2 given
BcMath\Number::__construct(): Argument #1 ($num) is not well-formed
