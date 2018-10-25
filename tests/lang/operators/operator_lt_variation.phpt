--TEST--
Test < operator : max int 32bit range
--FILE--
<?php

define("MAX_64Bit", 9223372036854775807);
define("MAX_32Bit", 2147483647);
define("MIN_64Bit", -9223372036854775807 - 1);
define("MIN_32Bit", -2147483647 - 1);

$validLessThan = array (
2147483646, array(MAX_32Bit, "2147483647", "2147483647.001", 2.147483647e9, 2147483647.9),
MIN_32Bit, array(MIN_32Bit + 1, "-2147483647", "-2147483646.001", -2.1474836461e9, -2147483646.9),
);

$invalidLessThan = array (
MAX_32Bit, array("2147483646", 2.1474836460001e9, MAX_32Bit - 1),
MIN_32Bit, array(MIN_32Bit - 1, "-2147483649", -2.1474836480001e9)
);

$failed = false;
// test for equality
for ($i = 0; $i < count($validLessThan); $i +=2) {
   $typeToTestVal = $validLessThan[$i];
   $compares = $validLessThan[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal < $compareVal) {
         // do nothing
      }
      else {
         echo "FAILED: '$typeToTestVal' >= '$compareVal'\n";
         $failed = true;
      }
   }
}
// test for invalid values
for ($i = 0; $i < count($invalidLessThan); $i +=2) {
   $typeToTestVal = $invalidLessThan[$i];
   $compares = $invalidLessThan[$i + 1];
   foreach($compares as $compareVal) {
      if ($typeToTestVal < $compareVal) {
         echo "FAILED: '$typeToTestVal' < '$compareVal'\n";
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
