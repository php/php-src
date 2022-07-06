--TEST--
Test deg2rad function : 64bit long tests
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
   var_dump(deg2rad($longVal));
}

?>
--EXPECT--
--- testing: 9223372036854775807 ---
float(1.6097821017949162E+17)
--- testing: -9223372036854775808 ---
float(-1.6097821017949162E+17)
--- testing: 2147483647 ---
float(37480660.27288565)
--- testing: -2147483648 ---
float(-37480660.29033894)
--- testing: 9223372034707292160 ---
float(1.6097821014201098E+17)
--- testing: -9223372034707292160 ---
float(-1.6097821014201098E+17)
--- testing: 2147483648 ---
float(37480660.29033894)
--- testing: -2147483649 ---
float(-37480660.30779223)
--- testing: 4294967294 ---
float(74961320.5457713)
--- testing: 4294967295 ---
float(74961320.5632246)
--- testing: 4294967293 ---
float(74961320.528318)
--- testing: 9223372036854775806 ---
float(1.6097821017949162E+17)
--- testing: 9.2233720368548E+18 ---
float(1.6097821017949162E+17)
--- testing: -9223372036854775807 ---
float(-1.6097821017949162E+17)
--- testing: -9.2233720368548E+18 ---
float(-1.6097821017949162E+17)
