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
--EXPECTF--
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

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(3) "abd"
--- testing: '123abc' ---

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(6) "123abd"
--- testing: '123e5' ---
float(12300001)
--- testing: '123e5xyz' ---

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(8) "123e5xza"
--- testing: ' 123abc' ---

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(7) " 123abd"
--- testing: '123 abc' ---

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(7) "123 abd"
--- testing: '123abc ' ---

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(7) "123abc "
--- testing: '3.4a' ---

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(4) "3.4b"
--- testing: 'a5.9' ---

Notice: String increment is deprecated, use str_inc() instead in %s on line 11
string(4) "a5.0"
===DONE===
