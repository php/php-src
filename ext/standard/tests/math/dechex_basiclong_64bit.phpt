--TEST--
Test dechex function : 64bit long tests
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
   var_dump(dechex($longVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: 9223372036854775807 ---
unicode(16) "7fffffffffffffff"
--- testing: -9223372036854775808 ---
unicode(16) "8000000000000000"
--- testing: 2147483647 ---
unicode(8) "7fffffff"
--- testing: -2147483648 ---
unicode(16) "ffffffff80000000"
--- testing: 9223372034707292160 ---
unicode(16) "7fffffff80000000"
--- testing: -9223372034707292160 ---
unicode(16) "8000000080000000"
--- testing: 2147483648 ---
unicode(8) "80000000"
--- testing: -2147483649 ---
unicode(16) "ffffffff7fffffff"
--- testing: 4294967294 ---
unicode(8) "fffffffe"
--- testing: 4294967295 ---
unicode(8) "ffffffff"
--- testing: 4294967293 ---
unicode(8) "fffffffd"
--- testing: 9223372036854775806 ---
unicode(16) "7ffffffffffffffe"
--- testing: 9.2233720368548E+18 ---
unicode(16) "8000000000000000"
--- testing: -9223372036854775807 ---
unicode(16) "8000000000000001"
--- testing: -9.2233720368548E+18 ---
unicode(16) "8000000000000000"
===DONE===
