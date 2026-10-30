--TEST--
BcMath\Number clone
--EXTENSIONS--
bcmath
--FILE--
<?php

$values = [
    '0',
    '0.0',
    '2',
    '1234',
    '12.0004',
    '0.1230',
    1,
    12345,
    '-0',
    '-0.0',
    '-2',
    '-1234',
    '-12.0004',
    '-0.1230',
    -1,
    -12345,
];

foreach ($values as $value) {
    $num = new BcMath\Number($value);
    $clone = clone $num;
    if ($num->value !== $clone->value || $num->scale !== $clone->scale || $num === $clone) {
        echo "Result is incorrect.\n";
        var_dump($num, $clone);
    }
}
echo 'done!';
?>
--EXPECT--
done!
