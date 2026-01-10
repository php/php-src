--TEST--
BcMath\Number ceil()
--EXTENSIONS--
bcmath
--FILE--
<?php
$nums = [
    '0',
    '0.00',
    '-0',
    '-0.00',
    '0.01',
    '0.000000000000000000000000000000000000000001',
    '-0.01',
    '-0.000000000000000000000000000000000000000001',
    '1',
    '1.0000',
    '1.0001',
    '100000.000000000000000000000000000000000000000001',
    '-1',
    '-1.0000',
    '-1.0001',
    '-100000.000000000000000000000000000000000000000001',
];

foreach ($nums as $num) {
    $func_ret = bcceil($num);
    $method_ret = (new BcMath\Number($num))->ceil();
    if ($method_ret->compare($func_ret, 0) !== 0) {
        echo "Result is incorrect.\n";
        var_dump($num, $func_ret, $method_ret);
    }
}
echo 'done!';
?>
--EXPECT--
done!
