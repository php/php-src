--TEST--
BcMath\Number powmod() with scale
--EXTENSIONS--
bcmath
--FILE--
<?php

$scales = [0, 10];

$values = ['12', '-12'];

$exponents = [
    [2, 'int'],
    ['0', 'string'],
    ['3', 'string'],
    [new BcMath\Number('2'), 'object'],
    [new BcMath\Number('0'), 'object'],
];

$mods = [
    [2, 'int'],
    ['3', 'string'],
    [new BcMath\Number('2'), 'object'],
];

foreach ($scales as $scale) {
    foreach ($values as $value) {
        $num = new BcMath\Number($value);

        foreach ($exponents as [$exponent, $type_ex]) {
            foreach ($mods as [$mod, $type_mod]) {
                $func_ret = bcpowmod($value, (string) $exponent, (string) $mod, $scale);
                $method_ret = $num->powmod($exponent, $mod, $scale);
                if ($method_ret->compare($func_ret) !== 0) {
                    echo "Result is incorrect.\n";
                    var_dump($value, $exponent, $mod, $scale, $func_ret, $method_ret);
                }
            }
        }
    }
}
echo 'done!';
?>
--EXPECT--
done!
