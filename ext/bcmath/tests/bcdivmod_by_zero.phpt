--TEST--
bcdivmod() function div by zero
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
require(__DIR__ . "/run_bcmath_tests_function.inc");

$dividends = [
    "15", "-15", "1", "-9", "14.14", "-16.60", "0.15", "-0.01",
    "15151324141414.412312232141241",
    "-132132245132134.1515123765412",
    "141241241241241248267654747412",
    "-149143276547656984948124912",
    "0.1322135476547459213732911312",
    "-0.123912932193769965476541321",
];

$divisors = [
    '0',
    '0.00',
];

foreach ($dividends as $firstTerm) {
    foreach ($divisors as $secondTerm) {
        try {
            bcdivmod($firstTerm, $secondTerm);
            echo "NG\n";
        } catch (Error $e) {
            echo $e->getMessage() === 'Division by zero' ? 'OK' :'NG';
            echo "\n";
        }
    }
}
?>
--EXPECT--
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
