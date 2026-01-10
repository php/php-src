--TEST--
BcMath\Number sqrt() with scale
--EXTENSIONS--
bcmath
--FILE--
<?php

$radicants = [
    "0",
    "0.00",
    "-0",
    "-0.00",
    "15151324141414.412312232141241",
    "141241241241241248267654747412",
    "0.1322135476547459213732911312",
    "14.14",
    "0.15",
    "15",
    "1",
];
$scales = [0, 10];

foreach ($scales as $scale) {
    foreach ($radicants as $radicant) {
        $func_ret = bcsqrt($radicant, $scale);
        $method_ret = (new BcMath\Number($radicant))->sqrt($scale);
        if ($method_ret->compare($func_ret) !== 0) {
            echo "Result is incorrect.\n";
            var_dump($radicant, $scale, $func_ret, $method_ret, ((string) $method_ret) === $func_ret);
        }
    }
}
echo 'done!';
?>
--EXPECT--
done!
