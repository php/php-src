--TEST--
Test | operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
	   echo "--- testing: '$strVal' | '$otherVal' ---\n";   
      var_dump(bin2hex($strVal|$otherVal));
   }
}

   
?>
===DONE===
--EXPECT--
--- testing: '0' | '0' ---
string(2) "30"
--- testing: '0' | '65' ---
string(4) "3635"
--- testing: '0' | '-44' ---
string(6) "3d3434"
--- testing: '0' | '1.2' ---
string(6) "312e32"
--- testing: '0' | '-7.7' ---
string(8) "3d372e37"
--- testing: '0' | 'abc' ---
string(6) "716263"
--- testing: '0' | '123abc' ---
string(12) "313233616263"
--- testing: '0' | '123e5' ---
string(10) "3132336535"
--- testing: '0' | '123e5xyz' ---
string(16) "313233653578797a"
--- testing: '0' | ' 123abc' ---
string(14) "30313233616263"
--- testing: '0' | '123 abc' ---
string(14) "31323320616263"
--- testing: '0' | '123abc ' ---
string(14) "31323361626320"
--- testing: '0' | '3.4a' ---
string(8) "332e3461"
--- testing: '0' | 'a5.9' ---
string(8) "71352e39"
--- testing: '65' | '0' ---
string(4) "3635"
--- testing: '65' | '65' ---
string(4) "3635"
--- testing: '65' | '-44' ---
string(6) "3f3534"
--- testing: '65' | '1.2' ---
string(6) "373f32"
--- testing: '65' | '-7.7' ---
string(8) "3f372e37"
--- testing: '65' | 'abc' ---
string(6) "777763"
--- testing: '65' | '123abc' ---
string(12) "373733616263"
--- testing: '65' | '123e5' ---
string(10) "3737336535"
--- testing: '65' | '123e5xyz' ---
string(16) "373733653578797a"
--- testing: '65' | ' 123abc' ---
string(14) "36353233616263"
--- testing: '65' | '123 abc' ---
string(14) "37373320616263"
--- testing: '65' | '123abc ' ---
string(14) "37373361626320"
--- testing: '65' | '3.4a' ---
string(8) "373f3461"
--- testing: '65' | 'a5.9' ---
string(8) "77352e39"
--- testing: '-44' | '0' ---
string(6) "3d3434"
--- testing: '-44' | '65' ---
string(6) "3f3534"
--- testing: '-44' | '-44' ---
string(6) "2d3434"
--- testing: '-44' | '1.2' ---
string(6) "3d3e36"
--- testing: '-44' | '-7.7' ---
string(8) "2d373e37"
--- testing: '-44' | 'abc' ---
string(6) "6d7677"
--- testing: '-44' | '123abc' ---
string(12) "3d3637616263"
--- testing: '-44' | '123e5' ---
string(10) "3d36376535"
--- testing: '-44' | '123e5xyz' ---
string(16) "3d3637653578797a"
--- testing: '-44' | ' 123abc' ---
string(14) "2d353633616263"
--- testing: '-44' | '123 abc' ---
string(14) "3d363720616263"
--- testing: '-44' | '123abc ' ---
string(14) "3d363761626320"
--- testing: '-44' | '3.4a' ---
string(8) "3f3e3461"
--- testing: '-44' | 'a5.9' ---
string(8) "6d353e39"
--- testing: '1.2' | '0' ---
string(6) "312e32"
--- testing: '1.2' | '65' ---
string(6) "373f32"
--- testing: '1.2' | '-44' ---
string(6) "3d3e36"
--- testing: '1.2' | '1.2' ---
string(6) "312e32"
--- testing: '1.2' | '-7.7' ---
string(8) "3d3f3e37"
--- testing: '1.2' | 'abc' ---
string(6) "716e73"
--- testing: '1.2' | '123abc' ---
string(12) "313e33616263"
--- testing: '1.2' | '123e5' ---
string(10) "313e336535"
--- testing: '1.2' | '123e5xyz' ---
string(16) "313e33653578797a"
--- testing: '1.2' | ' 123abc' ---
string(14) "313f3233616263"
--- testing: '1.2' | '123 abc' ---
string(14) "313e3320616263"
--- testing: '1.2' | '123abc ' ---
string(14) "313e3361626320"
--- testing: '1.2' | '3.4a' ---
string(8) "332e3661"
--- testing: '1.2' | 'a5.9' ---
string(8) "713f3e39"
--- testing: '-7.7' | '0' ---
string(8) "3d372e37"
--- testing: '-7.7' | '65' ---
string(8) "3f372e37"
--- testing: '-7.7' | '-44' ---
string(8) "2d373e37"
--- testing: '-7.7' | '1.2' ---
string(8) "3d3f3e37"
--- testing: '-7.7' | '-7.7' ---
string(8) "2d372e37"
--- testing: '-7.7' | 'abc' ---
string(8) "6d776f37"
--- testing: '-7.7' | '123abc' ---
string(12) "3d373f776263"
--- testing: '-7.7' | '123e5' ---
string(10) "3d373f7735"
--- testing: '-7.7' | '123e5xyz' ---
string(16) "3d373f773578797a"
--- testing: '-7.7' | ' 123abc' ---
string(14) "2d373e37616263"
--- testing: '-7.7' | '123 abc' ---
string(14) "3d373f37616263"
--- testing: '-7.7' | '123abc ' ---
string(14) "3d373f77626320"
--- testing: '-7.7' | '3.4a' ---
string(8) "3f3f3e77"
--- testing: '-7.7' | 'a5.9' ---
string(8) "6d372e3f"
--- testing: 'abc' | '0' ---
string(6) "716263"
--- testing: 'abc' | '65' ---
string(6) "777763"
--- testing: 'abc' | '-44' ---
string(6) "6d7677"
--- testing: 'abc' | '1.2' ---
string(6) "716e73"
--- testing: 'abc' | '-7.7' ---
string(8) "6d776f37"
--- testing: 'abc' | 'abc' ---
string(6) "616263"
--- testing: 'abc' | '123abc' ---
string(12) "717273616263"
--- testing: 'abc' | '123e5' ---
string(10) "7172736535"
--- testing: 'abc' | '123e5xyz' ---
string(16) "717273653578797a"
--- testing: 'abc' | ' 123abc' ---
string(14) "61737333616263"
--- testing: 'abc' | '123 abc' ---
string(14) "71727320616263"
--- testing: 'abc' | '123abc ' ---
string(14) "71727361626320"
--- testing: 'abc' | '3.4a' ---
string(8) "736e7761"
--- testing: 'abc' | 'a5.9' ---
string(8) "61776f39"
--- testing: '123abc' | '0' ---
string(12) "313233616263"
--- testing: '123abc' | '65' ---
string(12) "373733616263"
--- testing: '123abc' | '-44' ---
string(12) "3d3637616263"
--- testing: '123abc' | '1.2' ---
string(12) "313e33616263"
--- testing: '123abc' | '-7.7' ---
string(12) "3d373f776263"
--- testing: '123abc' | 'abc' ---
string(12) "717273616263"
--- testing: '123abc' | '123abc' ---
string(12) "313233616263"
--- testing: '123abc' | '123e5' ---
string(12) "313233657763"
--- testing: '123abc' | '123e5xyz' ---
string(16) "31323365777b797a"
--- testing: '123abc' | ' 123abc' ---
string(14) "31333373636363"
--- testing: '123abc' | '123 abc' ---
string(14) "31323361636363"
--- testing: '123abc' | '123abc ' ---
string(14) "31323361626320"
--- testing: '123abc' | '3.4a' ---
string(12) "333e37616263"
--- testing: '123abc' | 'a5.9' ---
string(12) "71373f796263"
--- testing: '123e5' | '0' ---
string(10) "3132336535"
--- testing: '123e5' | '65' ---
string(10) "3737336535"
--- testing: '123e5' | '-44' ---
string(10) "3d36376535"
--- testing: '123e5' | '1.2' ---
string(10) "313e336535"
--- testing: '123e5' | '-7.7' ---
string(10) "3d373f7735"
--- testing: '123e5' | 'abc' ---
string(10) "7172736535"
--- testing: '123e5' | '123abc' ---
string(12) "313233657763"
--- testing: '123e5' | '123e5' ---
string(10) "3132336535"
--- testing: '123e5' | '123e5xyz' ---
string(16) "313233653578797a"
--- testing: '123e5' | ' 123abc' ---
string(14) "31333377756263"
--- testing: '123e5' | '123 abc' ---
string(14) "31323365756263"
--- testing: '123e5' | '123abc ' ---
string(14) "31323365776320"
--- testing: '123e5' | '3.4a' ---
string(10) "333e376535"
--- testing: '123e5' | 'a5.9' ---
string(10) "71373f7d35"
--- testing: '123e5xyz' | '0' ---
string(16) "313233653578797a"
--- testing: '123e5xyz' | '65' ---
string(16) "373733653578797a"
--- testing: '123e5xyz' | '-44' ---
string(16) "3d3637653578797a"
--- testing: '123e5xyz' | '1.2' ---
string(16) "313e33653578797a"
--- testing: '123e5xyz' | '-7.7' ---
string(16) "3d373f773578797a"
--- testing: '123e5xyz' | 'abc' ---
string(16) "717273653578797a"
--- testing: '123e5xyz' | '123abc' ---
string(16) "31323365777b797a"
--- testing: '123e5xyz' | '123e5' ---
string(16) "313233653578797a"
--- testing: '123e5xyz' | '123e5xyz' ---
string(16) "313233653578797a"
--- testing: '123e5xyz' | ' 123abc' ---
string(16) "31333377757a7b7a"
--- testing: '123e5xyz' | '123 abc' ---
string(16) "31323365757a7b7a"
--- testing: '123e5xyz' | '123abc ' ---
string(16) "31323365777b797a"
--- testing: '123e5xyz' | '3.4a' ---
string(16) "333e37653578797a"
--- testing: '123e5xyz' | 'a5.9' ---
string(16) "71373f7d3578797a"
--- testing: ' 123abc' | '0' ---
string(14) "30313233616263"
--- testing: ' 123abc' | '65' ---
string(14) "36353233616263"
--- testing: ' 123abc' | '-44' ---
string(14) "2d353633616263"
--- testing: ' 123abc' | '1.2' ---
string(14) "313f3233616263"
--- testing: ' 123abc' | '-7.7' ---
string(14) "2d373e37616263"
--- testing: ' 123abc' | 'abc' ---
string(14) "61737333616263"
--- testing: ' 123abc' | '123abc' ---
string(14) "31333373636363"
--- testing: ' 123abc' | '123e5' ---
string(14) "31333377756263"
--- testing: ' 123abc' | '123e5xyz' ---
string(16) "31333377757a7b7a"
--- testing: ' 123abc' | ' 123abc' ---
string(14) "20313233616263"
--- testing: ' 123abc' | '123 abc' ---
string(14) "31333333616263"
--- testing: ' 123abc' | '123abc ' ---
string(14) "31333373636363"
--- testing: ' 123abc' | '3.4a' ---
string(14) "333f3673616263"
--- testing: ' 123abc' | 'a5.9' ---
string(14) "61353e3b616263"
--- testing: '123 abc' | '0' ---
string(14) "31323320616263"
--- testing: '123 abc' | '65' ---
string(14) "37373320616263"
--- testing: '123 abc' | '-44' ---
string(14) "3d363720616263"
--- testing: '123 abc' | '1.2' ---
string(14) "313e3320616263"
--- testing: '123 abc' | '-7.7' ---
string(14) "3d373f37616263"
--- testing: '123 abc' | 'abc' ---
string(14) "71727320616263"
--- testing: '123 abc' | '123abc' ---
string(14) "31323361636363"
--- testing: '123 abc' | '123e5' ---
string(14) "31323365756263"
--- testing: '123 abc' | '123e5xyz' ---
string(16) "31323365757a7b7a"
--- testing: '123 abc' | ' 123abc' ---
string(14) "31333333616263"
--- testing: '123 abc' | '123 abc' ---
string(14) "31323320616263"
--- testing: '123 abc' | '123abc ' ---
string(14) "31323361636363"
--- testing: '123 abc' | '3.4a' ---
string(14) "333e3761616263"
--- testing: '123 abc' | 'a5.9' ---
string(14) "71373f39616263"
--- testing: '123abc ' | '0' ---
string(14) "31323361626320"
--- testing: '123abc ' | '65' ---
string(14) "37373361626320"
--- testing: '123abc ' | '-44' ---
string(14) "3d363761626320"
--- testing: '123abc ' | '1.2' ---
string(14) "313e3361626320"
--- testing: '123abc ' | '-7.7' ---
string(14) "3d373f77626320"
--- testing: '123abc ' | 'abc' ---
string(14) "71727361626320"
--- testing: '123abc ' | '123abc' ---
string(14) "31323361626320"
--- testing: '123abc ' | '123e5' ---
string(14) "31323365776320"
--- testing: '123abc ' | '123e5xyz' ---
string(16) "31323365777b797a"
--- testing: '123abc ' | ' 123abc' ---
string(14) "31333373636363"
--- testing: '123abc ' | '123 abc' ---
string(14) "31323361636363"
--- testing: '123abc ' | '123abc ' ---
string(14) "31323361626320"
--- testing: '123abc ' | '3.4a' ---
string(14) "333e3761626320"
--- testing: '123abc ' | 'a5.9' ---
string(14) "71373f79626320"
--- testing: '3.4a' | '0' ---
string(8) "332e3461"
--- testing: '3.4a' | '65' ---
string(8) "373f3461"
--- testing: '3.4a' | '-44' ---
string(8) "3f3e3461"
--- testing: '3.4a' | '1.2' ---
string(8) "332e3661"
--- testing: '3.4a' | '-7.7' ---
string(8) "3f3f3e77"
--- testing: '3.4a' | 'abc' ---
string(8) "736e7761"
--- testing: '3.4a' | '123abc' ---
string(12) "333e37616263"
--- testing: '3.4a' | '123e5' ---
string(10) "333e376535"
--- testing: '3.4a' | '123e5xyz' ---
string(16) "333e37653578797a"
--- testing: '3.4a' | ' 123abc' ---
string(14) "333f3673616263"
--- testing: '3.4a' | '123 abc' ---
string(14) "333e3761616263"
--- testing: '3.4a' | '123abc ' ---
string(14) "333e3761626320"
--- testing: '3.4a' | '3.4a' ---
string(8) "332e3461"
--- testing: '3.4a' | 'a5.9' ---
string(8) "733f3e79"
--- testing: 'a5.9' | '0' ---
string(8) "71352e39"
--- testing: 'a5.9' | '65' ---
string(8) "77352e39"
--- testing: 'a5.9' | '-44' ---
string(8) "6d353e39"
--- testing: 'a5.9' | '1.2' ---
string(8) "713f3e39"
--- testing: 'a5.9' | '-7.7' ---
string(8) "6d372e3f"
--- testing: 'a5.9' | 'abc' ---
string(8) "61776f39"
--- testing: 'a5.9' | '123abc' ---
string(12) "71373f796263"
--- testing: 'a5.9' | '123e5' ---
string(10) "71373f7d35"
--- testing: 'a5.9' | '123e5xyz' ---
string(16) "71373f7d3578797a"
--- testing: 'a5.9' | ' 123abc' ---
string(14) "61353e3b616263"
--- testing: 'a5.9' | '123 abc' ---
string(14) "71373f39616263"
--- testing: 'a5.9' | '123abc ' ---
string(14) "71373f79626320"
--- testing: 'a5.9' | '3.4a' ---
string(8) "733f3e79"
--- testing: 'a5.9' | 'a5.9' ---
string(8) "61352e39"
===DONE===
