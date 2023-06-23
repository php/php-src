--TEST--
Test number_format() - test function with different decimal places
--FILE--
<?php
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
    -15151
);

$decimals = array(0, 1, 2, 3, 4, 5, -1, -2, -3, -4, -5);

foreach ($values as $value) {
    echo 'testing ';
    var_dump($value);

    foreach ($decimals as $decimal) {
        echo '... with decimal places of ' . $decimal . ': ';
        var_dump(number_format($value, $decimal));
    }
}

?>
--EXPECT--
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
