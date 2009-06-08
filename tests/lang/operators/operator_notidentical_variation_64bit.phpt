--TEST--
Test !== operator : max int 64bit range
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

$invalidNotIdentical = array (
MAX_32Bit, array(MAX_32Bit),
MIN_32Bit, array(MIN_32Bit),
MAX_64Bit, array(MAX_64Bit),
MIN_64Bit, array(MIN_64Bit),
);

$validNotIdentical = array (
MAX_32Bit, array("2147483647", "2147483647.0000000", 2.147483647e9, 2147483647.0, "2147483648", 2.1474836470001e9, MAX_32Bit - 1, MAX_32Bit + 1),
MIN_32Bit, array("-2147483648", "-2147483648.000", -2.147483648e9, -2147483648.0, "-2147483649", -2.1474836480001e9, MIN_32Bit -1, MIN_32Bit + 1),
MAX_64Bit, array(MAX_64Bit - 1, MAX_64Bit + 1),
MIN_64Bit, array(MIN_64Bit + 1, MIN_64Bit - 1),
);


$failed = false;
// test for valid values
for ($i = 0; $i < count($validNotIdentical); $i +=2) {
   $typeToTestVal = $validNotIdentical[$i];
   $compares = $validNotIdentical[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal !== $compareVal) {
         //Do Nothing
      }
      else {
         echo "FAILED: '$typeToTestVal' === '$compareVal'\n";
         $failed = true;
      }
   }
}
// test for invalid values
for ($i = 0; $i < count($invalidNotIdentical); $i +=2) {
   $typeToTestVal = $invalidNotIdentical[$i];
   $compares = $invalidNotIdentical[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal !== $compareVal) {
         echo "FAILED: '$typeToTestVal' !== '$compareVal'\n";
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