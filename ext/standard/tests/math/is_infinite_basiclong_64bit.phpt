--TEST--
Test is_infinite function : 64bit long tests
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
   var_dump(is_infinite($longVal));
}

?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
bool(false)
--- testing: -9223372036854775808 ---
bool(false)
--- testing: 2147483647 ---
bool(false)
--- testing: -2147483648 ---
bool(false)
--- testing: 9223372034707292160 ---
bool(false)
--- testing: -9223372034707292160 ---
bool(false)
--- testing: 2147483648 ---
bool(false)
--- testing: -2147483649 ---
bool(false)
--- testing: 4294967294 ---
bool(false)
--- testing: 4294967295 ---
bool(false)
--- testing: 4294967293 ---
bool(false)
--- testing: 9223372036854775806 ---
bool(false)
--- testing: 9.2233720368548E+18 ---
bool(false)
--- testing: -9223372036854775807 ---
bool(false)
--- testing: -9.2233720368548E+18 ---
bool(false)
===DONE===
