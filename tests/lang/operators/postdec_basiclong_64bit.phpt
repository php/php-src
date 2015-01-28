--TEST--
Test N-- operator : 64bit long and bigint tests
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
   $longVal--;
   var_dump($longVal);
}
   
?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
int(9223372036854775806)
--- testing: -9223372036854775808 ---
int(-9223372036854775809)
--- testing: 2147483647 ---
int(2147483646)
--- testing: -2147483648 ---
int(-2147483649)
--- testing: 9223372034707292160 ---
int(9223372034707292159)
--- testing: -9223372034707292160 ---
int(-9223372034707292161)
--- testing: 2147483648 ---
int(2147483647)
--- testing: -2147483649 ---
int(-2147483650)
--- testing: 4294967294 ---
int(4294967293)
--- testing: 4294967295 ---
int(4294967294)
--- testing: 4294967293 ---
int(4294967292)
--- testing: 9223372036854775806 ---
int(9223372036854775805)
--- testing: 9223372036854775808 ---
int(9223372036854775807)
--- testing: -9223372036854775807 ---
int(-9223372036854775808)
--- testing: -9223372036854775809 ---
int(-9223372036854775810)
===DONE===