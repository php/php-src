--TEST--
Test > operator : max int 32bit range
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php
 
define("MAX_64Bit", 9223372036854775807);
define("MAX_32Bit", 2147483647);
define("MIN_64Bit", -9223372036854775807 - 1);
define("MIN_32Bit", -2147483647 - 1);

$validGreaterThan = array (
MAX_32Bit, array(MAX_32Bit - 1, "2147483646", "2147483646.999", 2.147483646e9, 2147483646.9, MIN_32Bit),
-2147483647, array(MIN_32Bit, "-2147483648", "-2147483647.001", -2.1474836471e9, -2147483647.9),
);

$invalidGreaterThan = array (
MAX_32Bit, array(2e33, MAX_32Bit + 1),
MIN_32Bit, array(MIN_32Bit + 1, MAX_32Bit)
);



$failed = false;
// test valid values
for ($i = 0; $i < count($validGreaterThan); $i +=2) {
   $typeToTestVal = $validGreaterThan[$i];
   $compares = $validGreaterThan[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal > $compareVal) {
         // do nothing
      }
      else {
         echo "FAILED: '$typeToTestVal' <= '$compareVal'\n";
         $failed = true;
      }
   }
}
// test for invalid values
for ($i = 0; $i < count($invalidGreaterThan); $i +=2) {
   $typeToTestVal = $invalidGreaterThan[$i];
   $compares = $invalidGreaterThan[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal > $compareVal) {
         echo "FAILED: '$typeToTestVal' > '$compareVal'\n";
         $failed = true;
      }
   }
}

if ($failed == false) {
   echo "Test Passed\n";
}
   
?>
===DONE===
--EXPECT--
Test Passed
===DONE===
