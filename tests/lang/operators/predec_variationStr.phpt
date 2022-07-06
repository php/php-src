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
--EXPECT--
--- testing: '0' ---
int(-1)
--- testing: '65' ---
int(64)
--- testing: '-44' ---
int(-45)
--- testing: '1.2' ---
float(0.19999999999999996)
--- testing: '-7.7' ---
float(-8.7)
--- testing: 'abc' ---
string(3) "abc"
--- testing: '123abc' ---
string(6) "123abc"
--- testing: '123e5' ---
float(12299999)
--- testing: '123e5xyz' ---
string(8) "123e5xyz"
--- testing: ' 123abc' ---
string(7) " 123abc"
--- testing: '123 abc' ---
string(7) "123 abc"
--- testing: '123abc ' ---
string(7) "123abc "
--- testing: '3.4a' ---
string(4) "3.4a"
--- testing: 'a5.9' ---
string(4) "a5.9"
