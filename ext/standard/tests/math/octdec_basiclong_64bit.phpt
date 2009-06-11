--TEST--
Test octdec function : 64bit long tests
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

$octLongStrs = array(
   '777777777777777777777', 
   '1777777777777777777777', 
   '17777777777',
   '37777777777',
   '377777777777777777777777',
   '17777777777777777777777777',
   '377777777777',
   '777777777777',
);


foreach ($octLongStrs as $strVal) {
   echo "--- testing: $strVal ---\n";
   var_dump(octdec($strVal));
}
   
?>
===DONE===
--EXPECT--
--- testing: 777777777777777777777 ---
int(9223372036854775807)
--- testing: 1777777777777777777777 ---
float(1.844674407371E+19)
--- testing: 17777777777 ---
int(2147483647)
--- testing: 37777777777 ---
int(4294967295)
--- testing: 377777777777777777777777 ---
float(2.3611832414348E+21)
--- testing: 17777777777777777777777777 ---
float(7.5557863725914E+22)
--- testing: 377777777777 ---
int(34359738367)
--- testing: 777777777777 ---
int(68719476735)
===DONE===
