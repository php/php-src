--TEST--
Test >> operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
	   echo "--- testing: '$strVal' >> '$otherVal' ---\n";   
      var_dump(bin2hex($strVal>>$otherVal));
   }
}

   
?>
===DONE===
--EXPECT--
--- testing: '0' >> '0' ---
string(2) "30"
--- testing: '0' >> '65' ---
string(2) "30"
--- testing: '0' >> '-44' ---
string(2) "30"
--- testing: '0' >> '1.2' ---
string(2) "30"
--- testing: '0' >> '-7.7' ---
string(2) "30"
--- testing: '0' >> 'abc' ---
string(2) "30"
--- testing: '0' >> '123abc' ---
string(2) "30"
--- testing: '0' >> '123e5' ---
string(2) "30"
--- testing: '0' >> '123e5xyz' ---
string(2) "30"
--- testing: '0' >> ' 123abc' ---
string(2) "30"
--- testing: '0' >> '123 abc' ---
string(2) "30"
--- testing: '0' >> '123abc ' ---
string(2) "30"
--- testing: '0' >> '3.4a' ---
string(2) "30"
--- testing: '0' >> 'a5.9' ---
string(2) "30"
--- testing: '65' >> '0' ---
string(4) "3635"
--- testing: '65' >> '65' ---
string(4) "3332"
--- testing: '65' >> '-44' ---
string(2) "30"
--- testing: '65' >> '1.2' ---
string(4) "3332"
--- testing: '65' >> '-7.7' ---
string(2) "30"
--- testing: '65' >> 'abc' ---
string(4) "3635"
--- testing: '65' >> '123abc' ---
string(2) "30"
--- testing: '65' >> '123e5' ---
string(2) "30"
--- testing: '65' >> '123e5xyz' ---
string(2) "30"
--- testing: '65' >> ' 123abc' ---
string(2) "30"
--- testing: '65' >> '123 abc' ---
string(2) "30"
--- testing: '65' >> '123abc ' ---
string(2) "30"
--- testing: '65' >> '3.4a' ---
string(2) "38"
--- testing: '65' >> 'a5.9' ---
string(4) "3635"
--- testing: '-44' >> '0' ---
string(6) "2d3434"
--- testing: '-44' >> '65' ---
string(6) "2d3232"
--- testing: '-44' >> '-44' ---
string(4) "2d31"
--- testing: '-44' >> '1.2' ---
string(6) "2d3232"
--- testing: '-44' >> '-7.7' ---
string(4) "2d31"
--- testing: '-44' >> 'abc' ---
string(6) "2d3434"
--- testing: '-44' >> '123abc' ---
string(4) "2d31"
--- testing: '-44' >> '123e5' ---
string(4) "2d31"
--- testing: '-44' >> '123e5xyz' ---
string(4) "2d31"
--- testing: '-44' >> ' 123abc' ---
string(4) "2d31"
--- testing: '-44' >> '123 abc' ---
string(4) "2d31"
--- testing: '-44' >> '123abc ' ---
string(4) "2d31"
--- testing: '-44' >> '3.4a' ---
string(4) "2d36"
--- testing: '-44' >> 'a5.9' ---
string(6) "2d3434"
--- testing: '1.2' >> '0' ---
string(2) "31"
--- testing: '1.2' >> '65' ---
string(2) "30"
--- testing: '1.2' >> '-44' ---
string(2) "30"
--- testing: '1.2' >> '1.2' ---
string(2) "30"
--- testing: '1.2' >> '-7.7' ---
string(2) "30"
--- testing: '1.2' >> 'abc' ---
string(2) "31"
--- testing: '1.2' >> '123abc' ---
string(2) "30"
--- testing: '1.2' >> '123e5' ---
string(2) "30"
--- testing: '1.2' >> '123e5xyz' ---
string(2) "30"
--- testing: '1.2' >> ' 123abc' ---
string(2) "30"
--- testing: '1.2' >> '123 abc' ---
string(2) "30"
--- testing: '1.2' >> '123abc ' ---
string(2) "30"
--- testing: '1.2' >> '3.4a' ---
string(2) "30"
--- testing: '1.2' >> 'a5.9' ---
string(2) "31"
--- testing: '-7.7' >> '0' ---
string(4) "2d37"
--- testing: '-7.7' >> '65' ---
string(4) "2d34"
--- testing: '-7.7' >> '-44' ---
string(4) "2d31"
--- testing: '-7.7' >> '1.2' ---
string(4) "2d34"
--- testing: '-7.7' >> '-7.7' ---
string(4) "2d31"
--- testing: '-7.7' >> 'abc' ---
string(4) "2d37"
--- testing: '-7.7' >> '123abc' ---
string(4) "2d31"
--- testing: '-7.7' >> '123e5' ---
string(4) "2d31"
--- testing: '-7.7' >> '123e5xyz' ---
string(4) "2d31"
--- testing: '-7.7' >> ' 123abc' ---
string(4) "2d31"
--- testing: '-7.7' >> '123 abc' ---
string(4) "2d31"
--- testing: '-7.7' >> '123abc ' ---
string(4) "2d31"
--- testing: '-7.7' >> '3.4a' ---
string(4) "2d31"
--- testing: '-7.7' >> 'a5.9' ---
string(4) "2d37"
--- testing: 'abc' >> '0' ---
string(2) "30"
--- testing: 'abc' >> '65' ---
string(2) "30"
--- testing: 'abc' >> '-44' ---
string(2) "30"
--- testing: 'abc' >> '1.2' ---
string(2) "30"
--- testing: 'abc' >> '-7.7' ---
string(2) "30"
--- testing: 'abc' >> 'abc' ---
string(2) "30"
--- testing: 'abc' >> '123abc' ---
string(2) "30"
--- testing: 'abc' >> '123e5' ---
string(2) "30"
--- testing: 'abc' >> '123e5xyz' ---
string(2) "30"
--- testing: 'abc' >> ' 123abc' ---
string(2) "30"
--- testing: 'abc' >> '123 abc' ---
string(2) "30"
--- testing: 'abc' >> '123abc ' ---
string(2) "30"
--- testing: 'abc' >> '3.4a' ---
string(2) "30"
--- testing: 'abc' >> 'a5.9' ---
string(2) "30"
--- testing: '123abc' >> '0' ---
string(6) "313233"
--- testing: '123abc' >> '65' ---
string(4) "3631"
--- testing: '123abc' >> '-44' ---
string(2) "30"
--- testing: '123abc' >> '1.2' ---
string(4) "3631"
--- testing: '123abc' >> '-7.7' ---
string(2) "30"
--- testing: '123abc' >> 'abc' ---
string(6) "313233"
--- testing: '123abc' >> '123abc' ---
string(2) "30"
--- testing: '123abc' >> '123e5' ---
string(2) "30"
--- testing: '123abc' >> '123e5xyz' ---
string(2) "30"
--- testing: '123abc' >> ' 123abc' ---
string(2) "30"
--- testing: '123abc' >> '123 abc' ---
string(2) "30"
--- testing: '123abc' >> '123abc ' ---
string(2) "30"
--- testing: '123abc' >> '3.4a' ---
string(4) "3135"
--- testing: '123abc' >> 'a5.9' ---
string(6) "313233"
--- testing: '123e5' >> '0' ---
string(6) "313233"
--- testing: '123e5' >> '65' ---
string(4) "3631"
--- testing: '123e5' >> '-44' ---
string(2) "30"
--- testing: '123e5' >> '1.2' ---
string(4) "3631"
--- testing: '123e5' >> '-7.7' ---
string(2) "30"
--- testing: '123e5' >> 'abc' ---
string(6) "313233"
--- testing: '123e5' >> '123abc' ---
string(2) "30"
--- testing: '123e5' >> '123e5' ---
string(2) "30"
--- testing: '123e5' >> '123e5xyz' ---
string(2) "30"
--- testing: '123e5' >> ' 123abc' ---
string(2) "30"
--- testing: '123e5' >> '123 abc' ---
string(2) "30"
--- testing: '123e5' >> '123abc ' ---
string(2) "30"
--- testing: '123e5' >> '3.4a' ---
string(4) "3135"
--- testing: '123e5' >> 'a5.9' ---
string(6) "313233"
--- testing: '123e5xyz' >> '0' ---
string(6) "313233"
--- testing: '123e5xyz' >> '65' ---
string(4) "3631"
--- testing: '123e5xyz' >> '-44' ---
string(2) "30"
--- testing: '123e5xyz' >> '1.2' ---
string(4) "3631"
--- testing: '123e5xyz' >> '-7.7' ---
string(2) "30"
--- testing: '123e5xyz' >> 'abc' ---
string(6) "313233"
--- testing: '123e5xyz' >> '123abc' ---
string(2) "30"
--- testing: '123e5xyz' >> '123e5' ---
string(2) "30"
--- testing: '123e5xyz' >> '123e5xyz' ---
string(2) "30"
--- testing: '123e5xyz' >> ' 123abc' ---
string(2) "30"
--- testing: '123e5xyz' >> '123 abc' ---
string(2) "30"
--- testing: '123e5xyz' >> '123abc ' ---
string(2) "30"
--- testing: '123e5xyz' >> '3.4a' ---
string(4) "3135"
--- testing: '123e5xyz' >> 'a5.9' ---
string(6) "313233"
--- testing: ' 123abc' >> '0' ---
string(6) "313233"
--- testing: ' 123abc' >> '65' ---
string(4) "3631"
--- testing: ' 123abc' >> '-44' ---
string(2) "30"
--- testing: ' 123abc' >> '1.2' ---
string(4) "3631"
--- testing: ' 123abc' >> '-7.7' ---
string(2) "30"
--- testing: ' 123abc' >> 'abc' ---
string(6) "313233"
--- testing: ' 123abc' >> '123abc' ---
string(2) "30"
--- testing: ' 123abc' >> '123e5' ---
string(2) "30"
--- testing: ' 123abc' >> '123e5xyz' ---
string(2) "30"
--- testing: ' 123abc' >> ' 123abc' ---
string(2) "30"
--- testing: ' 123abc' >> '123 abc' ---
string(2) "30"
--- testing: ' 123abc' >> '123abc ' ---
string(2) "30"
--- testing: ' 123abc' >> '3.4a' ---
string(4) "3135"
--- testing: ' 123abc' >> 'a5.9' ---
string(6) "313233"
--- testing: '123 abc' >> '0' ---
string(6) "313233"
--- testing: '123 abc' >> '65' ---
string(4) "3631"
--- testing: '123 abc' >> '-44' ---
string(2) "30"
--- testing: '123 abc' >> '1.2' ---
string(4) "3631"
--- testing: '123 abc' >> '-7.7' ---
string(2) "30"
--- testing: '123 abc' >> 'abc' ---
string(6) "313233"
--- testing: '123 abc' >> '123abc' ---
string(2) "30"
--- testing: '123 abc' >> '123e5' ---
string(2) "30"
--- testing: '123 abc' >> '123e5xyz' ---
string(2) "30"
--- testing: '123 abc' >> ' 123abc' ---
string(2) "30"
--- testing: '123 abc' >> '123 abc' ---
string(2) "30"
--- testing: '123 abc' >> '123abc ' ---
string(2) "30"
--- testing: '123 abc' >> '3.4a' ---
string(4) "3135"
--- testing: '123 abc' >> 'a5.9' ---
string(6) "313233"
--- testing: '123abc ' >> '0' ---
string(6) "313233"
--- testing: '123abc ' >> '65' ---
string(4) "3631"
--- testing: '123abc ' >> '-44' ---
string(2) "30"
--- testing: '123abc ' >> '1.2' ---
string(4) "3631"
--- testing: '123abc ' >> '-7.7' ---
string(2) "30"
--- testing: '123abc ' >> 'abc' ---
string(6) "313233"
--- testing: '123abc ' >> '123abc' ---
string(2) "30"
--- testing: '123abc ' >> '123e5' ---
string(2) "30"
--- testing: '123abc ' >> '123e5xyz' ---
string(2) "30"
--- testing: '123abc ' >> ' 123abc' ---
string(2) "30"
--- testing: '123abc ' >> '123 abc' ---
string(2) "30"
--- testing: '123abc ' >> '123abc ' ---
string(2) "30"
--- testing: '123abc ' >> '3.4a' ---
string(4) "3135"
--- testing: '123abc ' >> 'a5.9' ---
string(6) "313233"
--- testing: '3.4a' >> '0' ---
string(2) "33"
--- testing: '3.4a' >> '65' ---
string(2) "31"
--- testing: '3.4a' >> '-44' ---
string(2) "30"
--- testing: '3.4a' >> '1.2' ---
string(2) "31"
--- testing: '3.4a' >> '-7.7' ---
string(2) "30"
--- testing: '3.4a' >> 'abc' ---
string(2) "33"
--- testing: '3.4a' >> '123abc' ---
string(2) "30"
--- testing: '3.4a' >> '123e5' ---
string(2) "30"
--- testing: '3.4a' >> '123e5xyz' ---
string(2) "30"
--- testing: '3.4a' >> ' 123abc' ---
string(2) "30"
--- testing: '3.4a' >> '123 abc' ---
string(2) "30"
--- testing: '3.4a' >> '123abc ' ---
string(2) "30"
--- testing: '3.4a' >> '3.4a' ---
string(2) "30"
--- testing: '3.4a' >> 'a5.9' ---
string(2) "33"
--- testing: 'a5.9' >> '0' ---
string(2) "30"
--- testing: 'a5.9' >> '65' ---
string(2) "30"
--- testing: 'a5.9' >> '-44' ---
string(2) "30"
--- testing: 'a5.9' >> '1.2' ---
string(2) "30"
--- testing: 'a5.9' >> '-7.7' ---
string(2) "30"
--- testing: 'a5.9' >> 'abc' ---
string(2) "30"
--- testing: 'a5.9' >> '123abc' ---
string(2) "30"
--- testing: 'a5.9' >> '123e5' ---
string(2) "30"
--- testing: 'a5.9' >> '123e5xyz' ---
string(2) "30"
--- testing: 'a5.9' >> ' 123abc' ---
string(2) "30"
--- testing: 'a5.9' >> '123 abc' ---
string(2) "30"
--- testing: 'a5.9' >> '123abc ' ---
string(2) "30"
--- testing: 'a5.9' >> '3.4a' ---
string(2) "30"
--- testing: 'a5.9' >> 'a5.9' ---
string(2) "30"
===DONE===
