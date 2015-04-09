--TEST--
Test << operator : various numbers as strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
?>
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
	   echo "--- testing: '$strVal' << '$otherVal' ---\n";   
      var_dump(strVal<<$otherVal);
   }
}
   
?>
===DONE===
--EXPECT--
--- testing: '0' << '0' ---
int(0)
--- testing: '0' << '65' ---
int(0)
--- testing: '0' << '-44' ---
bool(false)
--- testing: '0' << '1.2' ---
int(0)
--- testing: '0' << '-7.7' ---
bool(false)
--- testing: '0' << 'abc' ---
int(0)
--- testing: '0' << '123abc' ---
int(0)
--- testing: '0' << '123e5' ---
int(0)
--- testing: '0' << '123e5xyz' ---
int(0)
--- testing: '0' << ' 123abc' ---
int(0)
--- testing: '0' << '123 abc' ---
int(0)
--- testing: '0' << '123abc ' ---
int(0)
--- testing: '0' << '3.4a' ---
int(0)
--- testing: '0' << 'a5.9' ---
int(0)
--- testing: '65' << '0' ---
int(0)
--- testing: '65' << '65' ---
int(0)
--- testing: '65' << '-44' ---
bool(false)
--- testing: '65' << '1.2' ---
int(0)
--- testing: '65' << '-7.7' ---
bool(false)
--- testing: '65' << 'abc' ---
int(0)
--- testing: '65' << '123abc' ---
int(0)
--- testing: '65' << '123e5' ---
int(0)
--- testing: '65' << '123e5xyz' ---
int(0)
--- testing: '65' << ' 123abc' ---
int(0)
--- testing: '65' << '123 abc' ---
int(0)
--- testing: '65' << '123abc ' ---
int(0)
--- testing: '65' << '3.4a' ---
int(0)
--- testing: '65' << 'a5.9' ---
int(0)
--- testing: '-44' << '0' ---
int(0)
--- testing: '-44' << '65' ---
int(0)
--- testing: '-44' << '-44' ---
bool(false)
--- testing: '-44' << '1.2' ---
int(0)
--- testing: '-44' << '-7.7' ---
bool(false)
--- testing: '-44' << 'abc' ---
int(0)
--- testing: '-44' << '123abc' ---
int(0)
--- testing: '-44' << '123e5' ---
int(0)
--- testing: '-44' << '123e5xyz' ---
int(0)
--- testing: '-44' << ' 123abc' ---
int(0)
--- testing: '-44' << '123 abc' ---
int(0)
--- testing: '-44' << '123abc ' ---
int(0)
--- testing: '-44' << '3.4a' ---
int(0)
--- testing: '-44' << 'a5.9' ---
int(0)
--- testing: '1.2' << '0' ---
int(0)
--- testing: '1.2' << '65' ---
int(0)
--- testing: '1.2' << '-44' ---
bool(false)
--- testing: '1.2' << '1.2' ---
int(0)
--- testing: '1.2' << '-7.7' ---
bool(false)
--- testing: '1.2' << 'abc' ---
int(0)
--- testing: '1.2' << '123abc' ---
int(0)
--- testing: '1.2' << '123e5' ---
int(0)
--- testing: '1.2' << '123e5xyz' ---
int(0)
--- testing: '1.2' << ' 123abc' ---
int(0)
--- testing: '1.2' << '123 abc' ---
int(0)
--- testing: '1.2' << '123abc ' ---
int(0)
--- testing: '1.2' << '3.4a' ---
int(0)
--- testing: '1.2' << 'a5.9' ---
int(0)
--- testing: '-7.7' << '0' ---
int(0)
--- testing: '-7.7' << '65' ---
int(0)
--- testing: '-7.7' << '-44' ---
bool(false)
--- testing: '-7.7' << '1.2' ---
int(0)
--- testing: '-7.7' << '-7.7' ---
bool(false)
--- testing: '-7.7' << 'abc' ---
int(0)
--- testing: '-7.7' << '123abc' ---
int(0)
--- testing: '-7.7' << '123e5' ---
int(0)
--- testing: '-7.7' << '123e5xyz' ---
int(0)
--- testing: '-7.7' << ' 123abc' ---
int(0)
--- testing: '-7.7' << '123 abc' ---
int(0)
--- testing: '-7.7' << '123abc ' ---
int(0)
--- testing: '-7.7' << '3.4a' ---
int(0)
--- testing: '-7.7' << 'a5.9' ---
int(0)
--- testing: 'abc' << '0' ---
int(0)
--- testing: 'abc' << '65' ---
int(0)
--- testing: 'abc' << '-44' ---
bool(false)
--- testing: 'abc' << '1.2' ---
int(0)
--- testing: 'abc' << '-7.7' ---
bool(false)
--- testing: 'abc' << 'abc' ---
int(0)
--- testing: 'abc' << '123abc' ---
int(0)
--- testing: 'abc' << '123e5' ---
int(0)
--- testing: 'abc' << '123e5xyz' ---
int(0)
--- testing: 'abc' << ' 123abc' ---
int(0)
--- testing: 'abc' << '123 abc' ---
int(0)
--- testing: 'abc' << '123abc ' ---
int(0)
--- testing: 'abc' << '3.4a' ---
int(0)
--- testing: 'abc' << 'a5.9' ---
int(0)
--- testing: '123abc' << '0' ---
int(0)
--- testing: '123abc' << '65' ---
int(0)
--- testing: '123abc' << '-44' ---
bool(false)
--- testing: '123abc' << '1.2' ---
int(0)
--- testing: '123abc' << '-7.7' ---
bool(false)
--- testing: '123abc' << 'abc' ---
int(0)
--- testing: '123abc' << '123abc' ---
int(0)
--- testing: '123abc' << '123e5' ---
int(0)
--- testing: '123abc' << '123e5xyz' ---
int(0)
--- testing: '123abc' << ' 123abc' ---
int(0)
--- testing: '123abc' << '123 abc' ---
int(0)
--- testing: '123abc' << '123abc ' ---
int(0)
--- testing: '123abc' << '3.4a' ---
int(0)
--- testing: '123abc' << 'a5.9' ---
int(0)
--- testing: '123e5' << '0' ---
int(0)
--- testing: '123e5' << '65' ---
int(0)
--- testing: '123e5' << '-44' ---
bool(false)
--- testing: '123e5' << '1.2' ---
int(0)
--- testing: '123e5' << '-7.7' ---
bool(false)
--- testing: '123e5' << 'abc' ---
int(0)
--- testing: '123e5' << '123abc' ---
int(0)
--- testing: '123e5' << '123e5' ---
int(0)
--- testing: '123e5' << '123e5xyz' ---
int(0)
--- testing: '123e5' << ' 123abc' ---
int(0)
--- testing: '123e5' << '123 abc' ---
int(0)
--- testing: '123e5' << '123abc ' ---
int(0)
--- testing: '123e5' << '3.4a' ---
int(0)
--- testing: '123e5' << 'a5.9' ---
int(0)
--- testing: '123e5xyz' << '0' ---
int(0)
--- testing: '123e5xyz' << '65' ---
int(0)
--- testing: '123e5xyz' << '-44' ---
bool(false)
--- testing: '123e5xyz' << '1.2' ---
int(0)
--- testing: '123e5xyz' << '-7.7' ---
bool(false)
--- testing: '123e5xyz' << 'abc' ---
int(0)
--- testing: '123e5xyz' << '123abc' ---
int(0)
--- testing: '123e5xyz' << '123e5' ---
int(0)
--- testing: '123e5xyz' << '123e5xyz' ---
int(0)
--- testing: '123e5xyz' << ' 123abc' ---
int(0)
--- testing: '123e5xyz' << '123 abc' ---
int(0)
--- testing: '123e5xyz' << '123abc ' ---
int(0)
--- testing: '123e5xyz' << '3.4a' ---
int(0)
--- testing: '123e5xyz' << 'a5.9' ---
int(0)
--- testing: ' 123abc' << '0' ---
int(0)
--- testing: ' 123abc' << '65' ---
int(0)
--- testing: ' 123abc' << '-44' ---
bool(false)
--- testing: ' 123abc' << '1.2' ---
int(0)
--- testing: ' 123abc' << '-7.7' ---
bool(false)
--- testing: ' 123abc' << 'abc' ---
int(0)
--- testing: ' 123abc' << '123abc' ---
int(0)
--- testing: ' 123abc' << '123e5' ---
int(0)
--- testing: ' 123abc' << '123e5xyz' ---
int(0)
--- testing: ' 123abc' << ' 123abc' ---
int(0)
--- testing: ' 123abc' << '123 abc' ---
int(0)
--- testing: ' 123abc' << '123abc ' ---
int(0)
--- testing: ' 123abc' << '3.4a' ---
int(0)
--- testing: ' 123abc' << 'a5.9' ---
int(0)
--- testing: '123 abc' << '0' ---
int(0)
--- testing: '123 abc' << '65' ---
int(0)
--- testing: '123 abc' << '-44' ---
bool(false)
--- testing: '123 abc' << '1.2' ---
int(0)
--- testing: '123 abc' << '-7.7' ---
bool(false)
--- testing: '123 abc' << 'abc' ---
int(0)
--- testing: '123 abc' << '123abc' ---
int(0)
--- testing: '123 abc' << '123e5' ---
int(0)
--- testing: '123 abc' << '123e5xyz' ---
int(0)
--- testing: '123 abc' << ' 123abc' ---
int(0)
--- testing: '123 abc' << '123 abc' ---
int(0)
--- testing: '123 abc' << '123abc ' ---
int(0)
--- testing: '123 abc' << '3.4a' ---
int(0)
--- testing: '123 abc' << 'a5.9' ---
int(0)
--- testing: '123abc ' << '0' ---
int(0)
--- testing: '123abc ' << '65' ---
int(0)
--- testing: '123abc ' << '-44' ---
bool(false)
--- testing: '123abc ' << '1.2' ---
int(0)
--- testing: '123abc ' << '-7.7' ---
bool(false)
--- testing: '123abc ' << 'abc' ---
int(0)
--- testing: '123abc ' << '123abc' ---
int(0)
--- testing: '123abc ' << '123e5' ---
int(0)
--- testing: '123abc ' << '123e5xyz' ---
int(0)
--- testing: '123abc ' << ' 123abc' ---
int(0)
--- testing: '123abc ' << '123 abc' ---
int(0)
--- testing: '123abc ' << '123abc ' ---
int(0)
--- testing: '123abc ' << '3.4a' ---
int(0)
--- testing: '123abc ' << 'a5.9' ---
int(0)
--- testing: '3.4a' << '0' ---
int(0)
--- testing: '3.4a' << '65' ---
int(0)
--- testing: '3.4a' << '-44' ---
bool(false)
--- testing: '3.4a' << '1.2' ---
int(0)
--- testing: '3.4a' << '-7.7' ---
bool(false)
--- testing: '3.4a' << 'abc' ---
int(0)
--- testing: '3.4a' << '123abc' ---
int(0)
--- testing: '3.4a' << '123e5' ---
int(0)
--- testing: '3.4a' << '123e5xyz' ---
int(0)
--- testing: '3.4a' << ' 123abc' ---
int(0)
--- testing: '3.4a' << '123 abc' ---
int(0)
--- testing: '3.4a' << '123abc ' ---
int(0)
--- testing: '3.4a' << '3.4a' ---
int(0)
--- testing: '3.4a' << 'a5.9' ---
int(0)
--- testing: 'a5.9' << '0' ---
int(0)
--- testing: 'a5.9' << '65' ---
int(0)
--- testing: 'a5.9' << '-44' ---
bool(false)
--- testing: 'a5.9' << '1.2' ---
int(0)
--- testing: 'a5.9' << '-7.7' ---
bool(false)
--- testing: 'a5.9' << 'abc' ---
int(0)
--- testing: 'a5.9' << '123abc' ---
int(0)
--- testing: 'a5.9' << '123e5' ---
int(0)
--- testing: 'a5.9' << '123e5xyz' ---
int(0)
--- testing: 'a5.9' << ' 123abc' ---
int(0)
--- testing: 'a5.9' << '123 abc' ---
int(0)
--- testing: 'a5.9' << '123abc ' ---
int(0)
--- testing: 'a5.9' << '3.4a' ---
int(0)
--- testing: 'a5.9' << 'a5.9' ---
int(0)
===DONE===
