--TEST--
BcMath\Number pow() with scale
--EXTENSIONS--
bcmath
--FILE--
<?php

$scales = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

$values = ['12.5', '-12.5'];

$exponents = [
    [2, 'int'],
    [-3, 'int'],
    ['-2', 'string'],
    ['0', 'string'],
    [new BcMath\Number('2'), 'object'],
    [new BcMath\Number('-2'), 'object'],
    [new BcMath\Number('0'), 'object'],
];

foreach ($scales as $scale) {
    foreach ($values as $value) {
        $num = new BcMath\Number($value);

        foreach ($exponents as [$exponent, $type]) {
            $func_ret = bcpow($value, (string) $exponent, $scale);
            $method_ret = $num->pow($exponent, $scale);
            if ($method_ret->compare($func_ret) !== 0) {
                echo "Result is incorrect.\n";
                var_dump($value, $exponent, $scale, $func_ret, $method_ret);
            }
        }
    }
}
echo 'done!';
?>
--EXPECT--
done!
