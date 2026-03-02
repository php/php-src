--TEST--
BcMath\Number divmod() with scale
--EXTENSIONS--
bcmath
--FILE--
<?php

$scales = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10];

$values1 = ['100.012', '-100.012'];

$values2 = [
    [100, 'int'],
    [-30, 'int'],
    ['-20', 'string'],
    ['0.01', 'string'],
    ['-0.40', 'string'],
    [new BcMath\Number('80.3'), 'object'],
    [new BcMath\Number('-50.6'), 'object'],
];

foreach ($scales as $scale) {
    foreach ($values1 as $value1) {
        $num = new BcMath\Number($value1);

        foreach ($values2 as [$value2, $type]) {
            [$func_quot, $func_rem] = bcdivmod($value1, (string) $value2, $scale);
            [$method_quot, $method_rem] = $num->divmod($value2, $scale);
            if ($method_quot->compare($func_quot) !== 0) {
                echo "Quot is incorrect.\n";
                var_dump($value1, $value2, $scale, $func_quot, $method_quot);
            }
            if ($method_rem->compare($func_rem) !== 0) {
                echo "Rem is incorrect.\n";
                var_dump($value1, $value2, $scale, $func_rem, $method_rem);
            }
        }
    }
}
echo 'done!';
?>
--EXPECT--
done!
