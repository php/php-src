--TEST--
Test & operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   b"0",b"65",b"-44", b"1.2", b"-7.7", b"abc", b"123abc", b"123e5", b"123e5xyz", b" 123abc", b"123 abc", b"123abc ", b"3.4a",
   b"a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
	   echo "--- testing: '$strVal' & '$otherVal' ---\n";   
      var_dump(bin2hex($strVal&$otherVal));
   }
}

   
?>
===DONE===
--EXPECT--
--- testing: '0' & '0' ---
unicode(2) "30"
--- testing: '0' & '65' ---
unicode(2) "30"
--- testing: '0' & '-44' ---
unicode(2) "20"
--- testing: '0' & '1.2' ---
unicode(2) "30"
--- testing: '0' & '-7.7' ---
unicode(2) "20"
--- testing: '0' & 'abc' ---
unicode(2) "20"
--- testing: '0' & '123abc' ---
unicode(2) "30"
--- testing: '0' & '123e5' ---
unicode(2) "30"
--- testing: '0' & '123e5xyz' ---
unicode(2) "30"
--- testing: '0' & ' 123abc' ---
unicode(2) "20"
--- testing: '0' & '123 abc' ---
unicode(2) "30"
--- testing: '0' & '123abc ' ---
unicode(2) "30"
--- testing: '0' & '3.4a' ---
unicode(2) "30"
--- testing: '0' & 'a5.9' ---
unicode(2) "20"
--- testing: '65' & '0' ---
unicode(2) "30"
--- testing: '65' & '65' ---
unicode(4) "3635"
--- testing: '65' & '-44' ---
unicode(4) "2434"
--- testing: '65' & '1.2' ---
unicode(4) "3024"
--- testing: '65' & '-7.7' ---
unicode(4) "2435"
--- testing: '65' & 'abc' ---
unicode(4) "2020"
--- testing: '65' & '123abc' ---
unicode(4) "3030"
--- testing: '65' & '123e5' ---
unicode(4) "3030"
--- testing: '65' & '123e5xyz' ---
unicode(4) "3030"
--- testing: '65' & ' 123abc' ---
unicode(4) "2031"
--- testing: '65' & '123 abc' ---
unicode(4) "3030"
--- testing: '65' & '123abc ' ---
unicode(4) "3030"
--- testing: '65' & '3.4a' ---
unicode(4) "3224"
--- testing: '65' & 'a5.9' ---
unicode(4) "2035"
--- testing: '-44' & '0' ---
unicode(2) "20"
--- testing: '-44' & '65' ---
unicode(4) "2434"
--- testing: '-44' & '-44' ---
unicode(6) "2d3434"
--- testing: '-44' & '1.2' ---
unicode(6) "212430"
--- testing: '-44' & '-7.7' ---
unicode(6) "2d3424"
--- testing: '-44' & 'abc' ---
unicode(6) "212020"
--- testing: '-44' & '123abc' ---
unicode(6) "213030"
--- testing: '-44' & '123e5' ---
unicode(6) "213030"
--- testing: '-44' & '123e5xyz' ---
unicode(6) "213030"
--- testing: '-44' & ' 123abc' ---
unicode(6) "203030"
--- testing: '-44' & '123 abc' ---
unicode(6) "213030"
--- testing: '-44' & '123abc ' ---
unicode(6) "213030"
--- testing: '-44' & '3.4a' ---
unicode(6) "212434"
--- testing: '-44' & 'a5.9' ---
unicode(6) "213424"
--- testing: '1.2' & '0' ---
unicode(2) "30"
--- testing: '1.2' & '65' ---
unicode(4) "3024"
--- testing: '1.2' & '-44' ---
unicode(6) "212430"
--- testing: '1.2' & '1.2' ---
unicode(6) "312e32"
--- testing: '1.2' & '-7.7' ---
unicode(6) "212622"
--- testing: '1.2' & 'abc' ---
unicode(6) "212222"
--- testing: '1.2' & '123abc' ---
unicode(6) "312232"
--- testing: '1.2' & '123e5' ---
unicode(6) "312232"
--- testing: '1.2' & '123e5xyz' ---
unicode(6) "312232"
--- testing: '1.2' & ' 123abc' ---
unicode(6) "202032"
--- testing: '1.2' & '123 abc' ---
unicode(6) "312232"
--- testing: '1.2' & '123abc ' ---
unicode(6) "312232"
--- testing: '1.2' & '3.4a' ---
unicode(6) "312e30"
--- testing: '1.2' & 'a5.9' ---
unicode(6) "212422"
--- testing: '-7.7' & '0' ---
unicode(2) "20"
--- testing: '-7.7' & '65' ---
unicode(4) "2435"
--- testing: '-7.7' & '-44' ---
unicode(6) "2d3424"
--- testing: '-7.7' & '1.2' ---
unicode(6) "212622"
--- testing: '-7.7' & '-7.7' ---
unicode(8) "2d372e37"
--- testing: '-7.7' & 'abc' ---
unicode(6) "212222"
--- testing: '-7.7' & '123abc' ---
unicode(8) "21322221"
--- testing: '-7.7' & '123e5' ---
unicode(8) "21322225"
--- testing: '-7.7' & '123e5xyz' ---
unicode(8) "21322225"
--- testing: '-7.7' & ' 123abc' ---
unicode(8) "20312233"
--- testing: '-7.7' & '123 abc' ---
unicode(8) "21322220"
--- testing: '-7.7' & '123abc ' ---
unicode(8) "21322221"
--- testing: '-7.7' & '3.4a' ---
unicode(8) "21262421"
--- testing: '-7.7' & 'a5.9' ---
unicode(8) "21352e31"
--- testing: 'abc' & '0' ---
unicode(2) "20"
--- testing: 'abc' & '65' ---
unicode(4) "2020"
--- testing: 'abc' & '-44' ---
unicode(6) "212020"
--- testing: 'abc' & '1.2' ---
unicode(6) "212222"
--- testing: 'abc' & '-7.7' ---
unicode(6) "212222"
--- testing: 'abc' & 'abc' ---
unicode(6) "616263"
--- testing: 'abc' & '123abc' ---
unicode(6) "212223"
--- testing: 'abc' & '123e5' ---
unicode(6) "212223"
--- testing: 'abc' & '123e5xyz' ---
unicode(6) "212223"
--- testing: 'abc' & ' 123abc' ---
unicode(6) "202022"
--- testing: 'abc' & '123 abc' ---
unicode(6) "212223"
--- testing: 'abc' & '123abc ' ---
unicode(6) "212223"
--- testing: 'abc' & '3.4a' ---
unicode(6) "212220"
--- testing: 'abc' & 'a5.9' ---
unicode(6) "612022"
--- testing: '123abc' & '0' ---
unicode(2) "30"
--- testing: '123abc' & '65' ---
unicode(4) "3030"
--- testing: '123abc' & '-44' ---
unicode(6) "213030"
--- testing: '123abc' & '1.2' ---
unicode(6) "312232"
--- testing: '123abc' & '-7.7' ---
unicode(8) "21322221"
--- testing: '123abc' & 'abc' ---
unicode(6) "212223"
--- testing: '123abc' & '123abc' ---
unicode(12) "313233616263"
--- testing: '123abc' & '123e5' ---
unicode(10) "3132336120"
--- testing: '123abc' & '123e5xyz' ---
unicode(12) "313233612060"
--- testing: '123abc' & ' 123abc' ---
unicode(12) "203032216062"
--- testing: '123abc' & '123 abc' ---
unicode(12) "313233206062"
--- testing: '123abc' & '123abc ' ---
unicode(12) "313233616263"
--- testing: '123abc' & '3.4a' ---
unicode(8) "31223061"
--- testing: '123abc' & 'a5.9' ---
unicode(8) "21302221"
--- testing: '123e5' & '0' ---
unicode(2) "30"
--- testing: '123e5' & '65' ---
unicode(4) "3030"
--- testing: '123e5' & '-44' ---
unicode(6) "213030"
--- testing: '123e5' & '1.2' ---
unicode(6) "312232"
--- testing: '123e5' & '-7.7' ---
unicode(8) "21322225"
--- testing: '123e5' & 'abc' ---
unicode(6) "212223"
--- testing: '123e5' & '123abc' ---
unicode(10) "3132336120"
--- testing: '123e5' & '123e5' ---
unicode(10) "3132336535"
--- testing: '123e5' & '123e5xyz' ---
unicode(10) "3132336535"
--- testing: '123e5' & ' 123abc' ---
unicode(10) "2030322121"
--- testing: '123e5' & '123 abc' ---
unicode(10) "3132332021"
--- testing: '123e5' & '123abc ' ---
unicode(10) "3132336120"
--- testing: '123e5' & '3.4a' ---
unicode(8) "31223061"
--- testing: '123e5' & 'a5.9' ---
unicode(8) "21302221"
--- testing: '123e5xyz' & '0' ---
unicode(2) "30"
--- testing: '123e5xyz' & '65' ---
unicode(4) "3030"
--- testing: '123e5xyz' & '-44' ---
unicode(6) "213030"
--- testing: '123e5xyz' & '1.2' ---
unicode(6) "312232"
--- testing: '123e5xyz' & '-7.7' ---
unicode(8) "21322225"
--- testing: '123e5xyz' & 'abc' ---
unicode(6) "212223"
--- testing: '123e5xyz' & '123abc' ---
unicode(12) "313233612060"
--- testing: '123e5xyz' & '123e5' ---
unicode(10) "3132336535"
--- testing: '123e5xyz' & '123e5xyz' ---
unicode(16) "313233653578797a"
--- testing: '123e5xyz' & ' 123abc' ---
unicode(14) "20303221216061"
--- testing: '123e5xyz' & '123 abc' ---
unicode(14) "31323320216061"
--- testing: '123e5xyz' & '123abc ' ---
unicode(14) "31323361206020"
--- testing: '123e5xyz' & '3.4a' ---
unicode(8) "31223061"
--- testing: '123e5xyz' & 'a5.9' ---
unicode(8) "21302221"
--- testing: ' 123abc' & '0' ---
unicode(2) "20"
--- testing: ' 123abc' & '65' ---
unicode(4) "2031"
--- testing: ' 123abc' & '-44' ---
unicode(6) "203030"
--- testing: ' 123abc' & '1.2' ---
unicode(6) "202032"
--- testing: ' 123abc' & '-7.7' ---
unicode(8) "20312233"
--- testing: ' 123abc' & 'abc' ---
unicode(6) "202022"
--- testing: ' 123abc' & '123abc' ---
unicode(12) "203032216062"
--- testing: ' 123abc' & '123e5' ---
unicode(10) "2030322121"
--- testing: ' 123abc' & '123e5xyz' ---
unicode(14) "20303221216061"
--- testing: ' 123abc' & ' 123abc' ---
unicode(14) "20313233616263"
--- testing: ' 123abc' & '123 abc' ---
unicode(14) "20303220616263"
--- testing: ' 123abc' & '123abc ' ---
unicode(14) "20303221606220"
--- testing: ' 123abc' & '3.4a' ---
unicode(8) "20203021"
--- testing: ' 123abc' & 'a5.9' ---
unicode(8) "20312231"
--- testing: '123 abc' & '0' ---
unicode(2) "30"
--- testing: '123 abc' & '65' ---
unicode(4) "3030"
--- testing: '123 abc' & '-44' ---
unicode(6) "213030"
--- testing: '123 abc' & '1.2' ---
unicode(6) "312232"
--- testing: '123 abc' & '-7.7' ---
unicode(8) "21322220"
--- testing: '123 abc' & 'abc' ---
unicode(6) "212223"
--- testing: '123 abc' & '123abc' ---
unicode(12) "313233206062"
--- testing: '123 abc' & '123e5' ---
unicode(10) "3132332021"
--- testing: '123 abc' & '123e5xyz' ---
unicode(14) "31323320216061"
--- testing: '123 abc' & ' 123abc' ---
unicode(14) "20303220616263"
--- testing: '123 abc' & '123 abc' ---
unicode(14) "31323320616263"
--- testing: '123 abc' & '123abc ' ---
unicode(14) "31323320606220"
--- testing: '123 abc' & '3.4a' ---
unicode(8) "31223020"
--- testing: '123 abc' & 'a5.9' ---
unicode(8) "21302220"
--- testing: '123abc ' & '0' ---
unicode(2) "30"
--- testing: '123abc ' & '65' ---
unicode(4) "3030"
--- testing: '123abc ' & '-44' ---
unicode(6) "213030"
--- testing: '123abc ' & '1.2' ---
unicode(6) "312232"
--- testing: '123abc ' & '-7.7' ---
unicode(8) "21322221"
--- testing: '123abc ' & 'abc' ---
unicode(6) "212223"
--- testing: '123abc ' & '123abc' ---
unicode(12) "313233616263"
--- testing: '123abc ' & '123e5' ---
unicode(10) "3132336120"
--- testing: '123abc ' & '123e5xyz' ---
unicode(14) "31323361206020"
--- testing: '123abc ' & ' 123abc' ---
unicode(14) "20303221606220"
--- testing: '123abc ' & '123 abc' ---
unicode(14) "31323320606220"
--- testing: '123abc ' & '123abc ' ---
unicode(14) "31323361626320"
--- testing: '123abc ' & '3.4a' ---
unicode(8) "31223061"
--- testing: '123abc ' & 'a5.9' ---
unicode(8) "21302221"
--- testing: '3.4a' & '0' ---
unicode(2) "30"
--- testing: '3.4a' & '65' ---
unicode(4) "3224"
--- testing: '3.4a' & '-44' ---
unicode(6) "212434"
--- testing: '3.4a' & '1.2' ---
unicode(6) "312e30"
--- testing: '3.4a' & '-7.7' ---
unicode(8) "21262421"
--- testing: '3.4a' & 'abc' ---
unicode(6) "212220"
--- testing: '3.4a' & '123abc' ---
unicode(8) "31223061"
--- testing: '3.4a' & '123e5' ---
unicode(8) "31223061"
--- testing: '3.4a' & '123e5xyz' ---
unicode(8) "31223061"
--- testing: '3.4a' & ' 123abc' ---
unicode(8) "20203021"
--- testing: '3.4a' & '123 abc' ---
unicode(8) "31223020"
--- testing: '3.4a' & '123abc ' ---
unicode(8) "31223061"
--- testing: '3.4a' & '3.4a' ---
unicode(8) "332e3461"
--- testing: '3.4a' & 'a5.9' ---
unicode(8) "21242421"
--- testing: 'a5.9' & '0' ---
unicode(2) "20"
--- testing: 'a5.9' & '65' ---
unicode(4) "2035"
--- testing: 'a5.9' & '-44' ---
unicode(6) "213424"
--- testing: 'a5.9' & '1.2' ---
unicode(6) "212422"
--- testing: 'a5.9' & '-7.7' ---
unicode(8) "21352e31"
--- testing: 'a5.9' & 'abc' ---
unicode(6) "612022"
--- testing: 'a5.9' & '123abc' ---
unicode(8) "21302221"
--- testing: 'a5.9' & '123e5' ---
unicode(8) "21302221"
--- testing: 'a5.9' & '123e5xyz' ---
unicode(8) "21302221"
--- testing: 'a5.9' & ' 123abc' ---
unicode(8) "20312231"
--- testing: 'a5.9' & '123 abc' ---
unicode(8) "21302220"
--- testing: 'a5.9' & '123abc ' ---
unicode(8) "21302221"
--- testing: 'a5.9' & '3.4a' ---
unicode(8) "21242421"
--- testing: 'a5.9' & 'a5.9' ---
unicode(8) "61352e39"
===DONE===
