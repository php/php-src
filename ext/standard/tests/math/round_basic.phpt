--TEST--
Test round() - basic function test for round()
--INI--
precision=14
--FILE--
<?php
echo "*** Testing round() : basic functionality ***\n";

$values = [
    123456789,
    123.456789,
    -4.5679123,
    1.23E4,
    -4.567E3,
];

$precisions = [
    2,
    8,
    -2,
    -8,
];

foreach ($values as $value) {
    echo "round: $value\n";
    foreach ($precisions as $precision) {
        echo "....with precision $precision: ";
        $res = round($value, $precision);
        var_dump($res);
    }
}
?>
--EXPECT--
*** Testing round() : basic functionality ***
round: 123456789
....with precision 2: float(123456789)
....with precision 8: float(123456789)
....with precision -2: float(123456800)
....with precision -8: float(100000000)
round: 123.456789
....with precision 2: float(123.46)
....with precision 8: float(123.456789)
....with precision -2: float(100)
....with precision -8: float(0)
round: -4.5679123
....with precision 2: float(-4.57)
....with precision 8: float(-4.5679123)
....with precision -2: float(-0)
....with precision -8: float(-0)
round: 12300
....with precision 2: float(12300)
....with precision 8: float(12300)
....with precision -2: float(12300)
....with precision -8: float(0)
round: -4567
....with precision 2: float(-4567)
....with precision 8: float(-4567)
....with precision -2: float(-4600)
....with precision -8: float(-0)
