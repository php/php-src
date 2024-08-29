--TEST--
BcMath\Number construct error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    $num = new BcMath\Number('a');
    var_dump($num);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECT--
BcMath\Number::__construct(): Argument #1 ($num) is not well-formed
