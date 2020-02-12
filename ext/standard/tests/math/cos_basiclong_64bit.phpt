--TEST--
Test cos function : 64bit long tests
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
   var_dump(cos($longVal));
}

?>
--EXPECT--
--- testing: 9223372036854775807 ---
float(0.011800076512800236)
--- testing: -9223372036854775808 ---
float(0.011800076512800236)
--- testing: 2147483647 ---
float(-0.6888366918779438)
--- testing: -2147483648 ---
float(0.23781619457280337)
--- testing: 9223372034707292160 ---
float(-0.9684363006554391)
--- testing: -9223372034707292160 ---
float(-0.9684363006554391)
--- testing: 2147483648 ---
float(0.23781619457280337)
--- testing: -2147483649 ---
float(0.9458219684788874)
--- testing: 4294967294 ---
float(-0.051008023845301335)
--- testing: 4294967295 ---
float(-0.8679353473572022)
--- testing: 4294967293 ---
float(0.8128158415544156)
--- testing: 9223372036854775806 ---
float(0.011800076512800236)
--- testing: 9.2233720368548E+18 ---
float(0.011800076512800236)
--- testing: -9223372036854775807 ---
float(0.011800076512800236)
--- testing: -9.2233720368548E+18 ---
float(0.011800076512800236)
