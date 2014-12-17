--TEST--
Test --N operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);


foreach ($strVals as $strVal) {
   echo "--- testing: '$strVal' ---\n";
   var_dump(--$strVal);
}
   
?>
===DONE===
--EXPECT--
--- testing: '0' ---
int(-1)
--- testing: '65' ---
int(64)
--- testing: '-44' ---
int(-45)
--- testing: '1.2' ---
float(0.2)
--- testing: '-7.7' ---
float(-8.7)
--- testing: 'abc' ---
int(-1)
--- testing: '123abc' ---
int(122)
--- testing: '123e5' ---
float(12299999)
--- testing: '123e5xyz' ---
float(12299999)
--- testing: ' 123abc' ---
int(122)
--- testing: '123 abc' ---
int(122)
--- testing: '123abc ' ---
int(122)
--- testing: '3.4a' ---
float(2.4)
--- testing: 'a5.9' ---
int(-1)
===DONE===
