--TEST--
Test sin function : 64bit long tests
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
   var_dump(sin($longVal));
}

?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
float(0.99993037667344)
--- testing: -9223372036854775808 ---
float(-0.99993037667344)
--- testing: 2147483647 ---
float(-0.72491655514456)
--- testing: -2147483648 ---
float(0.97131017579294)
--- testing: 9223372034707292160 ---
float(0.24926117141025)
--- testing: -9223372034707292160 ---
float(-0.24926117141025)
--- testing: 2147483648 ---
float(-0.97131017579294)
--- testing: -2147483649 ---
float(0.32468570024367)
--- testing: 4294967294 ---
float(0.99869824346666)
--- testing: 4294967295 ---
float(0.49667719175329)
--- testing: 4294967293 ---
float(0.58252073586971)
--- testing: 9223372036854775806 ---
float(0.99993037667344)
--- testing: 9.2233720368548E+18 ---
float(0.99993037667344)
--- testing: -9223372036854775807 ---
float(-0.99993037667344)
--- testing: -9.2233720368548E+18 ---
float(-0.99993037667344)
===DONE===
