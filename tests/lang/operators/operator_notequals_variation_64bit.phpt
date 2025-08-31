--TEST--
Test == operator : max int 64bit range
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

$invalidNotEquals = array (
MAX_32Bit, array(MAX_32Bit, "2147483647", "2147483647.0000000", 2.147483647e9),
MIN_32Bit, array(MIN_32Bit, "-2147483648", "-2147483648.000", -2.147483648e9),
MAX_64Bit, array(MAX_64Bit, MAX_64Bit + 1),
MIN_64Bit, array(MIN_64Bit, MIN_64Bit - 1),
);

$validNotEquals = array (
MAX_32Bit, array("2147483648", 2.1474836470001e9, MAX_32Bit - 1, MAX_32Bit + 1),
MIN_32Bit, array("-2147483649", -2.1474836480001e9, MIN_32Bit -1, MIN_32Bit + 1),
MAX_64Bit, array(MAX_64Bit - 1),
MIN_64Bit, array(MIN_64Bit + 1),
);


$failed = false;
// test valid values
for ($i = 0; $i < count($validNotEquals); $i +=2) {
   $typeToTestVal = $validNotEquals[$i];
   $compares = $validNotEquals[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal != $compareVal && $typeToTestVal <> $compareVal) {
         // do nothing
      }
      else {
         echo "FAILED: '$typeToTestVal' == '$compareVal'\n";
         $failed = true;
      }
   }
}
// test invalid values
for ($i = 0; $i < count($invalidNotEquals); $i +=2) {
   $typeToTestVal = $invalidNotEquals[$i];
   $compares = $invalidNotEquals[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal != $compareVal || $typeToTestVal <> $compareVal) {
         echo "FAILED: '$typeToTestVal' != '$compareVal'\n";
         $failed = true;
      }
   }
}

if ($failed == false) {
   echo "Test Passed\n";
}

?>
--EXPECT--
Test Passed
