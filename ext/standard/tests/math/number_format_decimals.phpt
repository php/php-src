--TEST--
Test number_format() - test function with different decimal places
--FILE--
<?php

define("MAX_INT32", 2147483647);
define("MIN_INT32", -2147483647 - 1);

$values = array(
    1.5151,
    15.151,
    151.51,
    1515.1,
    15151,
    -1.5151,
    -15.151,
    -151.51,
    -1515.1,
    -15151,
    999,
    -999,
    999.0,
    -999.0,
    999999,
    -999999,
    999999.0,
    -999999.0,
    MAX_INT32,
    MIN_INT32,
);

$decimals = array(0, 1, 2, 3, 4, 5, -1, -2, -3, -4, -5, PHP_INT_MIN);

foreach ($values as $value) {
    echo 'testing ';
    var_dump($value);

    foreach ($decimals as $decimal) {
        echo '... with decimal places of ' . $decimal . ': ';
        var_dump(number_format($value, $decimal));
    }
}

?>
--EXPECTF--
testing float(1.5151)
... with decimal places of 0: string(1) "2"
... with decimal places of 1: string(3) "1.5"
... with decimal places of 2: string(4) "1.52"
... with decimal places of 3: string(5) "1.515"
... with decimal places of 4: string(6) "1.5151"
... with decimal places of 5: string(7) "1.51510"
... with decimal places of -1: string(1) "0"
... with decimal places of -2: string(1) "0"
... with decimal places of -3: string(1) "0"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(15.151)
... with decimal places of 0: string(2) "15"
... with decimal places of 1: string(4) "15.2"
... with decimal places of 2: string(5) "15.15"
... with decimal places of 3: string(6) "15.151"
... with decimal places of 4: string(7) "15.1510"
... with decimal places of 5: string(8) "15.15100"
... with decimal places of -1: string(2) "20"
... with decimal places of -2: string(1) "0"
... with decimal places of -3: string(1) "0"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(151.51)
... with decimal places of 0: string(3) "152"
... with decimal places of 1: string(5) "151.5"
... with decimal places of 2: string(6) "151.51"
... with decimal places of 3: string(7) "151.510"
... with decimal places of 4: string(8) "151.5100"
... with decimal places of 5: string(9) "151.51000"
... with decimal places of -1: string(3) "150"
... with decimal places of -2: string(3) "200"
... with decimal places of -3: string(1) "0"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(1515.1)
... with decimal places of 0: string(5) "1,515"
... with decimal places of 1: string(7) "1,515.1"
... with decimal places of 2: string(8) "1,515.10"
... with decimal places of 3: string(9) "1,515.100"
... with decimal places of 4: string(10) "1,515.1000"
... with decimal places of 5: string(11) "1,515.10000"
... with decimal places of -1: string(5) "1,520"
... with decimal places of -2: string(5) "1,500"
... with decimal places of -3: string(5) "2,000"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing int(15151)
... with decimal places of 0: string(6) "15,151"
... with decimal places of 1: string(8) "15,151.0"
... with decimal places of 2: string(9) "15,151.00"
... with decimal places of 3: string(10) "15,151.000"
... with decimal places of 4: string(11) "15,151.0000"
... with decimal places of 5: string(12) "15,151.00000"
... with decimal places of -1: string(6) "15,150"
... with decimal places of -2: string(6) "15,200"
... with decimal places of -3: string(6) "15,000"
... with decimal places of -4: string(6) "20,000"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(-1.5151)
... with decimal places of 0: string(2) "-2"
... with decimal places of 1: string(4) "-1.5"
... with decimal places of 2: string(5) "-1.52"
... with decimal places of 3: string(6) "-1.515"
... with decimal places of 4: string(7) "-1.5151"
... with decimal places of 5: string(8) "-1.51510"
... with decimal places of -1: string(1) "0"
... with decimal places of -2: string(1) "0"
... with decimal places of -3: string(1) "0"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(-15.151)
... with decimal places of 0: string(3) "-15"
... with decimal places of 1: string(5) "-15.2"
... with decimal places of 2: string(6) "-15.15"
... with decimal places of 3: string(7) "-15.151"
... with decimal places of 4: string(8) "-15.1510"
... with decimal places of 5: string(9) "-15.15100"
... with decimal places of -1: string(3) "-20"
... with decimal places of -2: string(1) "0"
... with decimal places of -3: string(1) "0"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(-151.51)
... with decimal places of 0: string(4) "-152"
... with decimal places of 1: string(6) "-151.5"
... with decimal places of 2: string(7) "-151.51"
... with decimal places of 3: string(8) "-151.510"
... with decimal places of 4: string(9) "-151.5100"
... with decimal places of 5: string(10) "-151.51000"
... with decimal places of -1: string(4) "-150"
... with decimal places of -2: string(4) "-200"
... with decimal places of -3: string(1) "0"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(-1515.1)
... with decimal places of 0: string(6) "-1,515"
... with decimal places of 1: string(8) "-1,515.1"
... with decimal places of 2: string(9) "-1,515.10"
... with decimal places of 3: string(10) "-1,515.100"
... with decimal places of 4: string(11) "-1,515.1000"
... with decimal places of 5: string(12) "-1,515.10000"
... with decimal places of -1: string(6) "-1,520"
... with decimal places of -2: string(6) "-1,500"
... with decimal places of -3: string(6) "-2,000"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing int(-15151)
... with decimal places of 0: string(7) "-15,151"
... with decimal places of 1: string(9) "-15,151.0"
... with decimal places of 2: string(10) "-15,151.00"
... with decimal places of 3: string(11) "-15,151.000"
... with decimal places of 4: string(12) "-15,151.0000"
... with decimal places of 5: string(13) "-15,151.00000"
... with decimal places of -1: string(7) "-15,150"
... with decimal places of -2: string(7) "-15,200"
... with decimal places of -3: string(7) "-15,000"
... with decimal places of -4: string(7) "-20,000"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing int(999)
... with decimal places of 0: string(3) "999"
... with decimal places of 1: string(5) "999.0"
... with decimal places of 2: string(6) "999.00"
... with decimal places of 3: string(7) "999.000"
... with decimal places of 4: string(8) "999.0000"
... with decimal places of 5: string(9) "999.00000"
... with decimal places of -1: string(5) "1,000"
... with decimal places of -2: string(5) "1,000"
... with decimal places of -3: string(5) "1,000"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing int(-999)
... with decimal places of 0: string(4) "-999"
... with decimal places of 1: string(6) "-999.0"
... with decimal places of 2: string(7) "-999.00"
... with decimal places of 3: string(8) "-999.000"
... with decimal places of 4: string(9) "-999.0000"
... with decimal places of 5: string(10) "-999.00000"
... with decimal places of -1: string(6) "-1,000"
... with decimal places of -2: string(6) "-1,000"
... with decimal places of -3: string(6) "-1,000"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(999)
... with decimal places of 0: string(3) "999"
... with decimal places of 1: string(5) "999.0"
... with decimal places of 2: string(6) "999.00"
... with decimal places of 3: string(7) "999.000"
... with decimal places of 4: string(8) "999.0000"
... with decimal places of 5: string(9) "999.00000"
... with decimal places of -1: string(5) "1,000"
... with decimal places of -2: string(5) "1,000"
... with decimal places of -3: string(5) "1,000"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing float(-999)
... with decimal places of 0: string(4) "-999"
... with decimal places of 1: string(6) "-999.0"
... with decimal places of 2: string(7) "-999.00"
... with decimal places of 3: string(8) "-999.000"
... with decimal places of 4: string(9) "-999.0000"
... with decimal places of 5: string(10) "-999.00000"
... with decimal places of -1: string(6) "-1,000"
... with decimal places of -2: string(6) "-1,000"
... with decimal places of -3: string(6) "-1,000"
... with decimal places of -4: string(1) "0"
... with decimal places of -5: string(1) "0"
... with decimal places of %i: string(1) "0"
testing int(999999)
... with decimal places of 0: string(7) "999,999"
... with decimal places of 1: string(9) "999,999.0"
... with decimal places of 2: string(10) "999,999.00"
... with decimal places of 3: string(11) "999,999.000"
... with decimal places of 4: string(12) "999,999.0000"
... with decimal places of 5: string(13) "999,999.00000"
... with decimal places of -1: string(9) "1,000,000"
... with decimal places of -2: string(9) "1,000,000"
... with decimal places of -3: string(9) "1,000,000"
... with decimal places of -4: string(9) "1,000,000"
... with decimal places of -5: string(9) "1,000,000"
... with decimal places of %i: string(1) "0"
testing int(-999999)
... with decimal places of 0: string(8) "-999,999"
... with decimal places of 1: string(10) "-999,999.0"
... with decimal places of 2: string(11) "-999,999.00"
... with decimal places of 3: string(12) "-999,999.000"
... with decimal places of 4: string(13) "-999,999.0000"
... with decimal places of 5: string(14) "-999,999.00000"
... with decimal places of -1: string(10) "-1,000,000"
... with decimal places of -2: string(10) "-1,000,000"
... with decimal places of -3: string(10) "-1,000,000"
... with decimal places of -4: string(10) "-1,000,000"
... with decimal places of -5: string(10) "-1,000,000"
... with decimal places of %i: string(1) "0"
testing float(999999)
... with decimal places of 0: string(7) "999,999"
... with decimal places of 1: string(9) "999,999.0"
... with decimal places of 2: string(10) "999,999.00"
... with decimal places of 3: string(11) "999,999.000"
... with decimal places of 4: string(12) "999,999.0000"
... with decimal places of 5: string(13) "999,999.00000"
... with decimal places of -1: string(9) "1,000,000"
... with decimal places of -2: string(9) "1,000,000"
... with decimal places of -3: string(9) "1,000,000"
... with decimal places of -4: string(9) "1,000,000"
... with decimal places of -5: string(9) "1,000,000"
... with decimal places of %i: string(1) "0"
testing float(-999999)
... with decimal places of 0: string(8) "-999,999"
... with decimal places of 1: string(10) "-999,999.0"
... with decimal places of 2: string(11) "-999,999.00"
... with decimal places of 3: string(12) "-999,999.000"
... with decimal places of 4: string(13) "-999,999.0000"
... with decimal places of 5: string(14) "-999,999.00000"
... with decimal places of -1: string(10) "-1,000,000"
... with decimal places of -2: string(10) "-1,000,000"
... with decimal places of -3: string(10) "-1,000,000"
... with decimal places of -4: string(10) "-1,000,000"
... with decimal places of -5: string(10) "-1,000,000"
... with decimal places of %i: string(1) "0"
testing int(2147483647)
... with decimal places of 0: string(13) "2,147,483,647"
... with decimal places of 1: string(15) "2,147,483,647.0"
... with decimal places of 2: string(16) "2,147,483,647.00"
... with decimal places of 3: string(17) "2,147,483,647.000"
... with decimal places of 4: string(18) "2,147,483,647.0000"
... with decimal places of 5: string(19) "2,147,483,647.00000"
... with decimal places of -1: string(13) "2,147,483,650"
... with decimal places of -2: string(13) "2,147,483,600"
... with decimal places of -3: string(13) "2,147,484,000"
... with decimal places of -4: string(13) "2,147,480,000"
... with decimal places of -5: string(13) "2,147,500,000"
... with decimal places of %i: string(1) "0"
testing int(-2147483648)
... with decimal places of 0: string(14) "-2,147,483,648"
... with decimal places of 1: string(16) "-2,147,483,648.0"
... with decimal places of 2: string(17) "-2,147,483,648.00"
... with decimal places of 3: string(18) "-2,147,483,648.000"
... with decimal places of 4: string(19) "-2,147,483,648.0000"
... with decimal places of 5: string(20) "-2,147,483,648.00000"
... with decimal places of -1: string(14) "-2,147,483,650"
... with decimal places of -2: string(14) "-2,147,483,600"
... with decimal places of -3: string(14) "-2,147,484,000"
... with decimal places of -4: string(14) "-2,147,480,000"
... with decimal places of -5: string(14) "-2,147,500,000"
... with decimal places of %i: string(1) "0"
