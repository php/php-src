--TEST--
Test - operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
	   echo "--- testing: '$strVal' - '$otherVal' ---\n";
      var_dump($strVal-$otherVal);
   }
}


?>
===DONE===
--EXPECT--
--- testing: '0' - '0' ---
int(0)
--- testing: '0' - '65' ---
int(-65)
--- testing: '0' - '-44' ---
int(44)
--- testing: '0' - '1.2' ---
float(-1.2)
--- testing: '0' - '-7.7' ---
float(7.7)
--- testing: '0' - 'abc' ---
int(0)
--- testing: '0' - '123abc' ---
int(-123)
--- testing: '0' - '123e5' ---
float(-12300000)
--- testing: '0' - '123e5xyz' ---
float(-12300000)
--- testing: '0' - ' 123abc' ---
int(-123)
--- testing: '0' - '123 abc' ---
int(-123)
--- testing: '0' - '123abc ' ---
int(-123)
--- testing: '0' - '3.4a' ---
float(-3.4)
--- testing: '0' - 'a5.9' ---
int(0)
--- testing: '65' - '0' ---
int(65)
--- testing: '65' - '65' ---
int(0)
--- testing: '65' - '-44' ---
int(109)
--- testing: '65' - '1.2' ---
float(63.8)
--- testing: '65' - '-7.7' ---
float(72.7)
--- testing: '65' - 'abc' ---
int(65)
--- testing: '65' - '123abc' ---
int(-58)
--- testing: '65' - '123e5' ---
float(-12299935)
--- testing: '65' - '123e5xyz' ---
float(-12299935)
--- testing: '65' - ' 123abc' ---
int(-58)
--- testing: '65' - '123 abc' ---
int(-58)
--- testing: '65' - '123abc ' ---
int(-58)
--- testing: '65' - '3.4a' ---
float(61.6)
--- testing: '65' - 'a5.9' ---
int(65)
--- testing: '-44' - '0' ---
int(-44)
--- testing: '-44' - '65' ---
int(-109)
--- testing: '-44' - '-44' ---
int(0)
--- testing: '-44' - '1.2' ---
float(-45.2)
--- testing: '-44' - '-7.7' ---
float(-36.3)
--- testing: '-44' - 'abc' ---
int(-44)
--- testing: '-44' - '123abc' ---
int(-167)
--- testing: '-44' - '123e5' ---
float(-12300044)
--- testing: '-44' - '123e5xyz' ---
float(-12300044)
--- testing: '-44' - ' 123abc' ---
int(-167)
--- testing: '-44' - '123 abc' ---
int(-167)
--- testing: '-44' - '123abc ' ---
int(-167)
--- testing: '-44' - '3.4a' ---
float(-47.4)
--- testing: '-44' - 'a5.9' ---
int(-44)
--- testing: '1.2' - '0' ---
float(1.2)
--- testing: '1.2' - '65' ---
float(-63.8)
--- testing: '1.2' - '-44' ---
float(45.2)
--- testing: '1.2' - '1.2' ---
float(0)
--- testing: '1.2' - '-7.7' ---
float(8.9)
--- testing: '1.2' - 'abc' ---
float(1.2)
--- testing: '1.2' - '123abc' ---
float(-121.8)
--- testing: '1.2' - '123e5' ---
float(-12299998.8)
--- testing: '1.2' - '123e5xyz' ---
float(-12299998.8)
--- testing: '1.2' - ' 123abc' ---
float(-121.8)
--- testing: '1.2' - '123 abc' ---
float(-121.8)
--- testing: '1.2' - '123abc ' ---
float(-121.8)
--- testing: '1.2' - '3.4a' ---
float(-2.2)
--- testing: '1.2' - 'a5.9' ---
float(1.2)
--- testing: '-7.7' - '0' ---
float(-7.7)
--- testing: '-7.7' - '65' ---
float(-72.7)
--- testing: '-7.7' - '-44' ---
float(36.3)
--- testing: '-7.7' - '1.2' ---
float(-8.9)
--- testing: '-7.7' - '-7.7' ---
float(0)
--- testing: '-7.7' - 'abc' ---
float(-7.7)
--- testing: '-7.7' - '123abc' ---
float(-130.7)
--- testing: '-7.7' - '123e5' ---
float(-12300007.7)
--- testing: '-7.7' - '123e5xyz' ---
float(-12300007.7)
--- testing: '-7.7' - ' 123abc' ---
float(-130.7)
--- testing: '-7.7' - '123 abc' ---
float(-130.7)
--- testing: '-7.7' - '123abc ' ---
float(-130.7)
--- testing: '-7.7' - '3.4a' ---
float(-11.1)
--- testing: '-7.7' - 'a5.9' ---
float(-7.7)
--- testing: 'abc' - '0' ---
int(0)
--- testing: 'abc' - '65' ---
int(-65)
--- testing: 'abc' - '-44' ---
int(44)
--- testing: 'abc' - '1.2' ---
float(-1.2)
--- testing: 'abc' - '-7.7' ---
float(7.7)
--- testing: 'abc' - 'abc' ---
int(0)
--- testing: 'abc' - '123abc' ---
int(-123)
--- testing: 'abc' - '123e5' ---
float(-12300000)
--- testing: 'abc' - '123e5xyz' ---
float(-12300000)
--- testing: 'abc' - ' 123abc' ---
int(-123)
--- testing: 'abc' - '123 abc' ---
int(-123)
--- testing: 'abc' - '123abc ' ---
int(-123)
--- testing: 'abc' - '3.4a' ---
float(-3.4)
--- testing: 'abc' - 'a5.9' ---
int(0)
--- testing: '123abc' - '0' ---
int(123)
--- testing: '123abc' - '65' ---
int(58)
--- testing: '123abc' - '-44' ---
int(167)
--- testing: '123abc' - '1.2' ---
float(121.8)
--- testing: '123abc' - '-7.7' ---
float(130.7)
--- testing: '123abc' - 'abc' ---
int(123)
--- testing: '123abc' - '123abc' ---
int(0)
--- testing: '123abc' - '123e5' ---
float(-12299877)
--- testing: '123abc' - '123e5xyz' ---
float(-12299877)
--- testing: '123abc' - ' 123abc' ---
int(0)
--- testing: '123abc' - '123 abc' ---
int(0)
--- testing: '123abc' - '123abc ' ---
int(0)
--- testing: '123abc' - '3.4a' ---
float(119.6)
--- testing: '123abc' - 'a5.9' ---
int(123)
--- testing: '123e5' - '0' ---
float(12300000)
--- testing: '123e5' - '65' ---
float(12299935)
--- testing: '123e5' - '-44' ---
float(12300044)
--- testing: '123e5' - '1.2' ---
float(12299998.8)
--- testing: '123e5' - '-7.7' ---
float(12300007.7)
--- testing: '123e5' - 'abc' ---
float(12300000)
--- testing: '123e5' - '123abc' ---
float(12299877)
--- testing: '123e5' - '123e5' ---
float(0)
--- testing: '123e5' - '123e5xyz' ---
float(0)
--- testing: '123e5' - ' 123abc' ---
float(12299877)
--- testing: '123e5' - '123 abc' ---
float(12299877)
--- testing: '123e5' - '123abc ' ---
float(12299877)
--- testing: '123e5' - '3.4a' ---
float(12299996.6)
--- testing: '123e5' - 'a5.9' ---
float(12300000)
--- testing: '123e5xyz' - '0' ---
float(12300000)
--- testing: '123e5xyz' - '65' ---
float(12299935)
--- testing: '123e5xyz' - '-44' ---
float(12300044)
--- testing: '123e5xyz' - '1.2' ---
float(12299998.8)
--- testing: '123e5xyz' - '-7.7' ---
float(12300007.7)
--- testing: '123e5xyz' - 'abc' ---
float(12300000)
--- testing: '123e5xyz' - '123abc' ---
float(12299877)
--- testing: '123e5xyz' - '123e5' ---
float(0)
--- testing: '123e5xyz' - '123e5xyz' ---
float(0)
--- testing: '123e5xyz' - ' 123abc' ---
float(12299877)
--- testing: '123e5xyz' - '123 abc' ---
float(12299877)
--- testing: '123e5xyz' - '123abc ' ---
float(12299877)
--- testing: '123e5xyz' - '3.4a' ---
float(12299996.6)
--- testing: '123e5xyz' - 'a5.9' ---
float(12300000)
--- testing: ' 123abc' - '0' ---
int(123)
--- testing: ' 123abc' - '65' ---
int(58)
--- testing: ' 123abc' - '-44' ---
int(167)
--- testing: ' 123abc' - '1.2' ---
float(121.8)
--- testing: ' 123abc' - '-7.7' ---
float(130.7)
--- testing: ' 123abc' - 'abc' ---
int(123)
--- testing: ' 123abc' - '123abc' ---
int(0)
--- testing: ' 123abc' - '123e5' ---
float(-12299877)
--- testing: ' 123abc' - '123e5xyz' ---
float(-12299877)
--- testing: ' 123abc' - ' 123abc' ---
int(0)
--- testing: ' 123abc' - '123 abc' ---
int(0)
--- testing: ' 123abc' - '123abc ' ---
int(0)
--- testing: ' 123abc' - '3.4a' ---
float(119.6)
--- testing: ' 123abc' - 'a5.9' ---
int(123)
--- testing: '123 abc' - '0' ---
int(123)
--- testing: '123 abc' - '65' ---
int(58)
--- testing: '123 abc' - '-44' ---
int(167)
--- testing: '123 abc' - '1.2' ---
float(121.8)
--- testing: '123 abc' - '-7.7' ---
float(130.7)
--- testing: '123 abc' - 'abc' ---
int(123)
--- testing: '123 abc' - '123abc' ---
int(0)
--- testing: '123 abc' - '123e5' ---
float(-12299877)
--- testing: '123 abc' - '123e5xyz' ---
float(-12299877)
--- testing: '123 abc' - ' 123abc' ---
int(0)
--- testing: '123 abc' - '123 abc' ---
int(0)
--- testing: '123 abc' - '123abc ' ---
int(0)
--- testing: '123 abc' - '3.4a' ---
float(119.6)
--- testing: '123 abc' - 'a5.9' ---
int(123)
--- testing: '123abc ' - '0' ---
int(123)
--- testing: '123abc ' - '65' ---
int(58)
--- testing: '123abc ' - '-44' ---
int(167)
--- testing: '123abc ' - '1.2' ---
float(121.8)
--- testing: '123abc ' - '-7.7' ---
float(130.7)
--- testing: '123abc ' - 'abc' ---
int(123)
--- testing: '123abc ' - '123abc' ---
int(0)
--- testing: '123abc ' - '123e5' ---
float(-12299877)
--- testing: '123abc ' - '123e5xyz' ---
float(-12299877)
--- testing: '123abc ' - ' 123abc' ---
int(0)
--- testing: '123abc ' - '123 abc' ---
int(0)
--- testing: '123abc ' - '123abc ' ---
int(0)
--- testing: '123abc ' - '3.4a' ---
float(119.6)
--- testing: '123abc ' - 'a5.9' ---
int(123)
--- testing: '3.4a' - '0' ---
float(3.4)
--- testing: '3.4a' - '65' ---
float(-61.6)
--- testing: '3.4a' - '-44' ---
float(47.4)
--- testing: '3.4a' - '1.2' ---
float(2.2)
--- testing: '3.4a' - '-7.7' ---
float(11.1)
--- testing: '3.4a' - 'abc' ---
float(3.4)
--- testing: '3.4a' - '123abc' ---
float(-119.6)
--- testing: '3.4a' - '123e5' ---
float(-12299996.6)
--- testing: '3.4a' - '123e5xyz' ---
float(-12299996.6)
--- testing: '3.4a' - ' 123abc' ---
float(-119.6)
--- testing: '3.4a' - '123 abc' ---
float(-119.6)
--- testing: '3.4a' - '123abc ' ---
float(-119.6)
--- testing: '3.4a' - '3.4a' ---
float(0)
--- testing: '3.4a' - 'a5.9' ---
float(3.4)
--- testing: 'a5.9' - '0' ---
int(0)
--- testing: 'a5.9' - '65' ---
int(-65)
--- testing: 'a5.9' - '-44' ---
int(44)
--- testing: 'a5.9' - '1.2' ---
float(-1.2)
--- testing: 'a5.9' - '-7.7' ---
float(7.7)
--- testing: 'a5.9' - 'abc' ---
int(0)
--- testing: 'a5.9' - '123abc' ---
int(-123)
--- testing: 'a5.9' - '123e5' ---
float(-12300000)
--- testing: 'a5.9' - '123e5xyz' ---
float(-12300000)
--- testing: 'a5.9' - ' 123abc' ---
int(-123)
--- testing: 'a5.9' - '123 abc' ---
int(-123)
--- testing: 'a5.9' - '123abc ' ---
int(-123)
--- testing: 'a5.9' - '3.4a' ---
float(-3.4)
--- testing: 'a5.9' - 'a5.9' ---
int(0)
===DONE===
