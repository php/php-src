--TEST--
BcMath\Number unserialize error
--EXTENSIONS--
bcmath
--FILE--
<?php
try {
    $num = new BcMath\Number(1);
    $num->__unserialize(['value' => '5']);
} catch (Error $e) {
    echo $e->getMessage() . "\n";
}

try {
    unserialize('O:13:"BcMath\Number":1:{s:5:"value";s:1:"a";}');
} catch (Exception $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Cannot modify readonly property BcMath\Number::$value
Invalid serialization data for BcMath\Number object
