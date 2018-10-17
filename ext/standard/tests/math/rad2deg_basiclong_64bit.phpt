--TEST--
Test rad2deg function : 64bit long tests
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


foreach ($longVals as $longVal) {
   echo "--- testing: $longVal ---\n";
   var_dump(rad2deg($longVal));
}

?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
float(5.2846029059076E+20)
--- testing: -9223372036854775808 ---
float(-5.2846029059076E+20)
--- testing: 2147483647 ---
float(123041749546.46)
--- testing: -2147483648 ---
float(-123041749603.76)
--- testing: 9223372034707292160 ---
float(5.2846029046772E+20)
--- testing: -9223372034707292160 ---
float(-5.2846029046772E+20)
--- testing: 2147483648 ---
float(123041749603.76)
--- testing: -2147483649 ---
float(-123041749661.05)
--- testing: 4294967294 ---
float(246083499092.92)
--- testing: 4294967295 ---
float(246083499150.22)
--- testing: 4294967293 ---
float(246083499035.63)
--- testing: 9223372036854775806 ---
float(5.2846029059076E+20)
--- testing: 9.2233720368548E+18 ---
float(5.2846029059076E+20)
--- testing: -9223372036854775807 ---
float(-5.2846029059076E+20)
--- testing: -9.2233720368548E+18 ---
float(-5.2846029059076E+20)
===DONE===
