--TEST--
Test number_format function : 64bit long tests
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
    5,
    0,
    -1,
    -5,
    -10,
    -11,
    -17,
    -19,
    -20,
    PHP_INT_MIN,
);

foreach ($longVals as $longVal) {
    echo "--- testing: ";
    var_dump($longVal);
    foreach ($precisions as $precision) {
        echo "... with precision " . $precision . ": ";
        var_dump(number_format($longVal, $precision));
    }
}

?>
--EXPECT--
--- testing: int(9223372036854775807)
... with precision 5: string(31) "9,223,372,036,854,775,807.00000"
... with precision 0: string(25) "9,223,372,036,854,775,807"
... with precision -1: string(25) "9,223,372,036,854,775,810"
... with precision -5: string(25) "9,223,372,036,854,800,000"
... with precision -10: string(25) "9,223,372,040,000,000,000"
... with precision -11: string(25) "9,223,372,000,000,000,000"
... with precision -17: string(25) "9,200,000,000,000,000,000"
... with precision -19: string(26) "10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(-9223372036854775808)
... with precision 5: string(32) "-9,223,372,036,854,775,808.00000"
... with precision 0: string(26) "-9,223,372,036,854,775,808"
... with precision -1: string(26) "-9,223,372,036,854,775,810"
... with precision -5: string(26) "-9,223,372,036,854,800,000"
... with precision -10: string(26) "-9,223,372,040,000,000,000"
... with precision -11: string(26) "-9,223,372,000,000,000,000"
... with precision -17: string(26) "-9,200,000,000,000,000,000"
... with precision -19: string(27) "-10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(2147483647)
... with precision 5: string(19) "2,147,483,647.00000"
... with precision 0: string(13) "2,147,483,647"
... with precision -1: string(13) "2,147,483,650"
... with precision -5: string(13) "2,147,500,000"
... with precision -10: string(1) "0"
... with precision -11: string(1) "0"
... with precision -17: string(1) "0"
... with precision -19: string(1) "0"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(-2147483648)
... with precision 5: string(20) "-2,147,483,648.00000"
... with precision 0: string(14) "-2,147,483,648"
... with precision -1: string(14) "-2,147,483,650"
... with precision -5: string(14) "-2,147,500,000"
... with precision -10: string(1) "0"
... with precision -11: string(1) "0"
... with precision -17: string(1) "0"
... with precision -19: string(1) "0"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(9223372034707292160)
... with precision 5: string(31) "9,223,372,034,707,292,160.00000"
... with precision 0: string(25) "9,223,372,034,707,292,160"
... with precision -1: string(25) "9,223,372,034,707,292,160"
... with precision -5: string(25) "9,223,372,034,707,300,000"
... with precision -10: string(25) "9,223,372,030,000,000,000"
... with precision -11: string(25) "9,223,372,000,000,000,000"
... with precision -17: string(25) "9,200,000,000,000,000,000"
... with precision -19: string(26) "10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(-9223372034707292160)
... with precision 5: string(32) "-9,223,372,034,707,292,160.00000"
... with precision 0: string(26) "-9,223,372,034,707,292,160"
... with precision -1: string(26) "-9,223,372,034,707,292,160"
... with precision -5: string(26) "-9,223,372,034,707,300,000"
... with precision -10: string(26) "-9,223,372,030,000,000,000"
... with precision -11: string(26) "-9,223,372,000,000,000,000"
... with precision -17: string(26) "-9,200,000,000,000,000,000"
... with precision -19: string(27) "-10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(2147483648)
... with precision 5: string(19) "2,147,483,648.00000"
... with precision 0: string(13) "2,147,483,648"
... with precision -1: string(13) "2,147,483,650"
... with precision -5: string(13) "2,147,500,000"
... with precision -10: string(1) "0"
... with precision -11: string(1) "0"
... with precision -17: string(1) "0"
... with precision -19: string(1) "0"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(-2147483649)
... with precision 5: string(20) "-2,147,483,649.00000"
... with precision 0: string(14) "-2,147,483,649"
... with precision -1: string(14) "-2,147,483,650"
... with precision -5: string(14) "-2,147,500,000"
... with precision -10: string(1) "0"
... with precision -11: string(1) "0"
... with precision -17: string(1) "0"
... with precision -19: string(1) "0"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(4294967294)
... with precision 5: string(19) "4,294,967,294.00000"
... with precision 0: string(13) "4,294,967,294"
... with precision -1: string(13) "4,294,967,290"
... with precision -5: string(13) "4,295,000,000"
... with precision -10: string(1) "0"
... with precision -11: string(1) "0"
... with precision -17: string(1) "0"
... with precision -19: string(1) "0"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(4294967295)
... with precision 5: string(19) "4,294,967,295.00000"
... with precision 0: string(13) "4,294,967,295"
... with precision -1: string(13) "4,294,967,300"
... with precision -5: string(13) "4,295,000,000"
... with precision -10: string(1) "0"
... with precision -11: string(1) "0"
... with precision -17: string(1) "0"
... with precision -19: string(1) "0"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(4294967293)
... with precision 5: string(19) "4,294,967,293.00000"
... with precision 0: string(13) "4,294,967,293"
... with precision -1: string(13) "4,294,967,290"
... with precision -5: string(13) "4,295,000,000"
... with precision -10: string(1) "0"
... with precision -11: string(1) "0"
... with precision -17: string(1) "0"
... with precision -19: string(1) "0"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(9223372036854775806)
... with precision 5: string(31) "9,223,372,036,854,775,806.00000"
... with precision 0: string(25) "9,223,372,036,854,775,806"
... with precision -1: string(25) "9,223,372,036,854,775,810"
... with precision -5: string(25) "9,223,372,036,854,800,000"
... with precision -10: string(25) "9,223,372,040,000,000,000"
... with precision -11: string(25) "9,223,372,000,000,000,000"
... with precision -17: string(25) "9,200,000,000,000,000,000"
... with precision -19: string(26) "10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: float(9.223372036854776E+18)
... with precision 5: string(31) "9,223,372,036,854,775,808.00000"
... with precision 0: string(25) "9,223,372,036,854,775,808"
... with precision -1: string(25) "9,223,372,036,854,775,808"
... with precision -5: string(25) "9,223,372,036,854,800,384"
... with precision -10: string(25) "9,223,372,040,000,000,000"
... with precision -11: string(25) "9,223,372,000,000,000,000"
... with precision -17: string(25) "9,200,000,000,000,000,000"
... with precision -19: string(26) "10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: int(-9223372036854775807)
... with precision 5: string(32) "-9,223,372,036,854,775,807.00000"
... with precision 0: string(26) "-9,223,372,036,854,775,807"
... with precision -1: string(26) "-9,223,372,036,854,775,810"
... with precision -5: string(26) "-9,223,372,036,854,800,000"
... with precision -10: string(26) "-9,223,372,040,000,000,000"
... with precision -11: string(26) "-9,223,372,000,000,000,000"
... with precision -17: string(26) "-9,200,000,000,000,000,000"
... with precision -19: string(27) "-10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
--- testing: float(-9.223372036854776E+18)
... with precision 5: string(32) "-9,223,372,036,854,775,808.00000"
... with precision 0: string(26) "-9,223,372,036,854,775,808"
... with precision -1: string(26) "-9,223,372,036,854,775,808"
... with precision -5: string(26) "-9,223,372,036,854,800,384"
... with precision -10: string(26) "-9,223,372,040,000,000,000"
... with precision -11: string(26) "-9,223,372,000,000,000,000"
... with precision -17: string(26) "-9,200,000,000,000,000,000"
... with precision -19: string(27) "-10,000,000,000,000,000,000"
... with precision -20: string(1) "0"
... with precision -9223372036854775808: string(1) "0"
