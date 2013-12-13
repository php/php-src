--TEST--
Test N++ operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);


foreach ($strVals as $strVal) {
   echo "--- testing: '$strVal' ---\n";
   $strVal++;
   var_dump($strVal);
}
   
?>
===DONE===
--EXPECT--
--- testing: '0' ---
int(1)
--- testing: '65' ---
int(66)
--- testing: '-44' ---
int(-43)
--- testing: '1.2' ---
float(2.2)
--- testing: '-7.7' ---
float(-6.7)
--- testing: 'abc' ---
int(1)
--- testing: '123abc' ---
int(124)
--- testing: '123e5' ---
float(12300001)
--- testing: '123e5xyz' ---
float(12300001)
--- testing: ' 123abc' ---
int(124)
--- testing: '123 abc' ---
int(124)
--- testing: '123abc ' ---
int(124)
--- testing: '3.4a' ---
float(4.4)
--- testing: 'a5.9' ---
int(1)
===DONE===
