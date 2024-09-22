--TEST--
BcMath\Number round()
--EXTENSIONS--
bcmath
--FILE--
<?php
foreach (RoundingMode::cases() as $mode) {
    foreach ([
        '0.1',
        '-0.1',
        '1.0',
        '-1.0',
        '1.2',
        '-1.2',
        '1.7',
        '-1.7',
        '1.5',
        '-1.5',
        '2.5',
        '-2.5',
    ] as $number) {
        $func_ret = bcround($number, 0, $mode);
        $method_ret = (new BcMath\Number($number))->round(0, $mode);
        if ($method_ret->compare($func_ret) !== 0) {
            echo "Result is incorrect.\n";
            var_dump($number, $mode, $func_ret, $method_ret);
        }
    }
}

foreach (RoundingMode::cases() as $mode) {
    foreach ([
        '1.2345678',
        '-1.2345678',
    ] as $number) {
        $func_ret = bcround($number, 5, $mode);
        $method_ret = (new BcMath\Number($number))->round(5, $mode);
        if ($method_ret->compare($func_ret) !== 0) {
            echo "Result is incorrect.\n";
            var_dump($number, $mode, $func_ret, $method_ret);
        }
    }
}

echo 'done!';
?>
--EXPECT--
done!
