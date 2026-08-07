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
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo "\n";
$cases = [
    'O:13:"BcMath\Number":1:{s:5:"value";s:1:"a";}',
    'O:13:"BcMath\Number":1:{s:5:"value";s:0:"";}',
    'O:13:"BcMath\Number":0:{}',
    'O:13:"BcMath\Number":1:{s:5:"value";i:1;}',
];

foreach ($cases as $case) {
    try {
        unserialize($case);
    } catch (Exception $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
}
?>
--EXPECT--
Error: Cannot modify readonly property BcMath\Number::$value

Exception: Invalid serialization data for BcMath\Number object
Exception: Invalid serialization data for BcMath\Number object
Exception: Invalid serialization data for BcMath\Number object
Exception: Invalid serialization data for BcMath\Number object
