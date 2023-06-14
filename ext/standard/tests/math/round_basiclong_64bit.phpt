--TEST--
Test round function : 64bit long tests
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php

define("MAX_64Bit", 9223372036854775807);
define("MAX_32Bit", 2147483647);
define("MIN_64Bit", -9223372036854775807 - 1);
define("MIN_32Bit", -2147483647 - 1);

$longVals = array(
    MAX_64Bit, MIN_64Bit, MAX_32Bit, MIN_32Bit, MAX_64Bit - MAX_32Bit, MIN_64Bit - MIN_32Bit,
    MAX_32Bit + 1, MIN_32Bit - 1, MAX_32Bit * 2, (MAX_32Bit * 2) + 1, (MAX_32Bit * 2) - 1,
    MAX_64Bit -1, MAX_64Bit + 1, MIN_64Bit + 1, MIN_64Bit - 1
);

$precisions = array(
    0,
    -1,
    -5,
    -10,
    -11,
    -17,
    -19,
    -20,
);

foreach ($longVals as $longVal) {
    echo "--- testing: ";
    var_dump($longVal);
    foreach ($precisions as $precision) {
        echo "... with precision " . $precision . ": ";
        var_dump(round($longVal, $precision));
    }
}

?>
--EXPECT--
--- testing: int(9223372036854775807)
... with precision 0: int(9223372036854775807)
... with precision -1: float(9.223372036854776E+18)
... with precision -5: float(9.2233720368548E+18)
... with precision -10: float(9.22337204E+18)
... with precision -11: int(9223372000000000000)
... with precision -17: int(9200000000000000000)
... with precision -19: float(1.0E+19)
... with precision -20: int(0)
--- testing: int(-9223372036854775808)
... with precision 0: int(-9223372036854775808)
... with precision -1: float(-9.223372036854776E+18)
... with precision -5: float(-9.2233720368548E+18)
... with precision -10: float(-9.22337204E+18)
... with precision -11: int(-9223372000000000000)
... with precision -17: int(-9200000000000000000)
... with precision -19: float(-1.0E+19)
... with precision -20: int(0)
--- testing: int(2147483647)
... with precision 0: int(2147483647)
... with precision -1: int(2147483650)
... with precision -5: int(2147500000)
... with precision -10: int(0)
... with precision -11: int(0)
... with precision -17: int(0)
... with precision -19: int(0)
... with precision -20: int(0)
--- testing: int(-2147483648)
... with precision 0: int(-2147483648)
... with precision -1: int(-2147483650)
... with precision -5: int(-2147500000)
... with precision -10: int(0)
... with precision -11: int(0)
... with precision -17: int(0)
... with precision -19: int(0)
... with precision -20: int(0)
--- testing: int(9223372034707292160)
... with precision 0: int(9223372034707292160)
... with precision -1: int(9223372034707292160)
... with precision -5: int(9223372034707300000)
... with precision -10: int(9223372030000000000)
... with precision -11: int(9223372000000000000)
... with precision -17: int(9200000000000000000)
... with precision -19: float(1.0E+19)
... with precision -20: int(0)
--- testing: int(-9223372034707292160)
... with precision 0: int(-9223372034707292160)
... with precision -1: int(-9223372034707292160)
... with precision -5: int(-9223372034707300000)
... with precision -10: int(-9223372030000000000)
... with precision -11: int(-9223372000000000000)
... with precision -17: int(-9200000000000000000)
... with precision -19: float(-1.0E+19)
... with precision -20: int(0)
--- testing: int(2147483648)
... with precision 0: int(2147483648)
... with precision -1: int(2147483650)
... with precision -5: int(2147500000)
... with precision -10: int(0)
... with precision -11: int(0)
... with precision -17: int(0)
... with precision -19: int(0)
... with precision -20: int(0)
--- testing: int(-2147483649)
... with precision 0: int(-2147483649)
... with precision -1: int(-2147483650)
... with precision -5: int(-2147500000)
... with precision -10: int(0)
... with precision -11: int(0)
... with precision -17: int(0)
... with precision -19: int(0)
... with precision -20: int(0)
--- testing: int(4294967294)
... with precision 0: int(4294967294)
... with precision -1: int(4294967290)
... with precision -5: int(4295000000)
... with precision -10: int(0)
... with precision -11: int(0)
... with precision -17: int(0)
... with precision -19: int(0)
... with precision -20: int(0)
--- testing: int(4294967295)
... with precision 0: int(4294967295)
... with precision -1: int(4294967300)
... with precision -5: int(4295000000)
... with precision -10: int(0)
... with precision -11: int(0)
... with precision -17: int(0)
... with precision -19: int(0)
... with precision -20: int(0)
--- testing: int(4294967293)
... with precision 0: int(4294967293)
... with precision -1: int(4294967290)
... with precision -5: int(4295000000)
... with precision -10: int(0)
... with precision -11: int(0)
... with precision -17: int(0)
... with precision -19: int(0)
... with precision -20: int(0)
--- testing: int(9223372036854775806)
... with precision 0: int(9223372036854775806)
... with precision -1: float(9.223372036854776E+18)
... with precision -5: float(9.2233720368548E+18)
... with precision -10: float(9.22337204E+18)
... with precision -11: int(9223372000000000000)
... with precision -17: int(9200000000000000000)
... with precision -19: float(1.0E+19)
... with precision -20: int(0)
--- testing: float(9.223372036854776E+18)
... with precision 0: float(9.223372036854776E+18)
... with precision -1: float(9.223372036854776E+18)
... with precision -5: float(9.2233720368548E+18)
... with precision -10: float(9.22337204E+18)
... with precision -11: float(9.223372E+18)
... with precision -17: float(9.2E+18)
... with precision -19: float(1.0E+19)
... with precision -20: float(0)
--- testing: int(-9223372036854775807)
... with precision 0: int(-9223372036854775807)
... with precision -1: float(-9.223372036854776E+18)
... with precision -5: float(-9.2233720368548E+18)
... with precision -10: float(-9.22337204E+18)
... with precision -11: int(-9223372000000000000)
... with precision -17: int(-9200000000000000000)
... with precision -19: float(-1.0E+19)
... with precision -20: int(0)
--- testing: float(-9.223372036854776E+18)
... with precision 0: float(-9.223372036854776E+18)
... with precision -1: float(-9.223372036854776E+18)
... with precision -5: float(-9.2233720368548E+18)
... with precision -10: float(-9.22337204E+18)
... with precision -11: float(-9.223372E+18)
... with precision -17: float(-9.2E+18)
... with precision -19: float(-1.0E+19)
... with precision -20: float(-0)
