--TEST--
Test << operator : various numbers as strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 4) die("skip this test is for 32bit platform only");
if ((65<<65)==0) die("skip this test is for Intel only");
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
      var_dump(bin2hex($strVal<<$otherVal));
   }
}
   
?>
===DONE===
--EXPECT--
--- testing: '0' << '0' ---
string(2) "30"
--- testing: '0' << '65' ---
string(2) "30"
--- testing: '0' << '-44' ---
string(2) "30"
--- testing: '0' << '1.2' ---
string(2) "30"
--- testing: '0' << '-7.7' ---
string(2) "30"
--- testing: '0' << 'abc' ---
string(2) "30"
--- testing: '0' << '123abc' ---
string(2) "30"
--- testing: '0' << '123e5' ---
string(2) "30"
--- testing: '0' << '123e5xyz' ---
string(2) "30"
--- testing: '0' << ' 123abc' ---
string(2) "30"
--- testing: '0' << '123 abc' ---
string(2) "30"
--- testing: '0' << '123abc ' ---
string(2) "30"
--- testing: '0' << '3.4a' ---
string(2) "30"
--- testing: '0' << 'a5.9' ---
string(2) "30"
--- testing: '65' << '0' ---
string(4) "3635"
--- testing: '65' << '65' ---
string(6) "313330"
--- testing: '65' << '-44' ---
string(16) "3638313537343430"
--- testing: '65' << '1.2' ---
string(6) "313330"
--- testing: '65' << '-7.7' ---
string(22) "2d32313133393239323136"
--- testing: '65' << 'abc' ---
string(4) "3635"
--- testing: '65' << '123abc' ---
string(18) "313334323137373238"
--- testing: '65' << '123e5' ---
string(18) "313334323137373238"
--- testing: '65' << '123e5xyz' ---
string(18) "313334323137373238"
--- testing: '65' << ' 123abc' ---
string(18) "313334323137373238"
--- testing: '65' << '123 abc' ---
string(18) "313334323137373238"
--- testing: '65' << '123abc ' ---
string(18) "313334323137373238"
--- testing: '65' << '3.4a' ---
string(6) "353230"
--- testing: '65' << 'a5.9' ---
string(4) "3635"
--- testing: '-44' << '0' ---
string(6) "2d3434"
--- testing: '-44' << '65' ---
string(6) "2d3838"
--- testing: '-44' << '-44' ---
string(18) "2d3436313337333434"
--- testing: '-44' << '1.2' ---
string(6) "2d3838"
--- testing: '-44' << '-7.7' ---
string(22) "2d31343736333935303038"
--- testing: '-44' << 'abc' ---
string(6) "2d3434"
--- testing: '-44' << '123abc' ---
string(22) "2d31363130363132373336"
--- testing: '-44' << '123e5' ---
string(22) "2d31363130363132373336"
--- testing: '-44' << '123e5xyz' ---
string(22) "2d31363130363132373336"
--- testing: '-44' << ' 123abc' ---
string(22) "2d31363130363132373336"
--- testing: '-44' << '123 abc' ---
string(22) "2d31363130363132373336"
--- testing: '-44' << '123abc ' ---
string(22) "2d31363130363132373336"
--- testing: '-44' << '3.4a' ---
string(8) "2d333532"
--- testing: '-44' << 'a5.9' ---
string(6) "2d3434"
--- testing: '1.2' << '0' ---
string(2) "31"
--- testing: '1.2' << '65' ---
string(2) "32"
--- testing: '1.2' << '-44' ---
string(14) "31303438353736"
--- testing: '1.2' << '1.2' ---
string(2) "32"
--- testing: '1.2' << '-7.7' ---
string(16) "3333353534343332"
--- testing: '1.2' << 'abc' ---
string(2) "31"
--- testing: '1.2' << '123abc' ---
string(18) "313334323137373238"
--- testing: '1.2' << '123e5' ---
string(18) "313334323137373238"
--- testing: '1.2' << '123e5xyz' ---
string(18) "313334323137373238"
--- testing: '1.2' << ' 123abc' ---
string(18) "313334323137373238"
--- testing: '1.2' << '123 abc' ---
string(18) "313334323137373238"
--- testing: '1.2' << '123abc ' ---
string(18) "313334323137373238"
--- testing: '1.2' << '3.4a' ---
string(2) "38"
--- testing: '1.2' << 'a5.9' ---
string(2) "31"
--- testing: '-7.7' << '0' ---
string(4) "2d37"
--- testing: '-7.7' << '65' ---
string(6) "2d3134"
--- testing: '-7.7' << '-44' ---
string(16) "2d37333430303332"
--- testing: '-7.7' << '1.2' ---
string(6) "2d3134"
--- testing: '-7.7' << '-7.7' ---
string(20) "2d323334383831303234"
--- testing: '-7.7' << 'abc' ---
string(4) "2d37"
--- testing: '-7.7' << '123abc' ---
string(20) "2d393339353234303936"
--- testing: '-7.7' << '123e5' ---
string(20) "2d393339353234303936"
--- testing: '-7.7' << '123e5xyz' ---
string(20) "2d393339353234303936"
--- testing: '-7.7' << ' 123abc' ---
string(20) "2d393339353234303936"
--- testing: '-7.7' << '123 abc' ---
string(20) "2d393339353234303936"
--- testing: '-7.7' << '123abc ' ---
string(20) "2d393339353234303936"
--- testing: '-7.7' << '3.4a' ---
string(6) "2d3536"
--- testing: '-7.7' << 'a5.9' ---
string(4) "2d37"
--- testing: 'abc' << '0' ---
string(2) "30"
--- testing: 'abc' << '65' ---
string(2) "30"
--- testing: 'abc' << '-44' ---
string(2) "30"
--- testing: 'abc' << '1.2' ---
string(2) "30"
--- testing: 'abc' << '-7.7' ---
string(2) "30"
--- testing: 'abc' << 'abc' ---
string(2) "30"
--- testing: 'abc' << '123abc' ---
string(2) "30"
--- testing: 'abc' << '123e5' ---
string(2) "30"
--- testing: 'abc' << '123e5xyz' ---
string(2) "30"
--- testing: 'abc' << ' 123abc' ---
string(2) "30"
--- testing: 'abc' << '123 abc' ---
string(2) "30"
--- testing: 'abc' << '123abc ' ---
string(2) "30"
--- testing: 'abc' << '3.4a' ---
string(2) "30"
--- testing: 'abc' << 'a5.9' ---
string(2) "30"
--- testing: '123abc' << '0' ---
string(6) "313233"
--- testing: '123abc' << '65' ---
string(6) "323436"
--- testing: '123abc' << '-44' ---
string(18) "313238393734383438"
--- testing: '123abc' << '1.2' ---
string(6) "323436"
--- testing: '123abc' << '-7.7' ---
string(20) "2d313637373732313630"
--- testing: '123abc' << 'abc' ---
string(6) "313233"
--- testing: '123abc' << '123abc' ---
string(20) "2d363731303838363430"
--- testing: '123abc' << '123e5' ---
string(20) "2d363731303838363430"
--- testing: '123abc' << '123e5xyz' ---
string(20) "2d363731303838363430"
--- testing: '123abc' << ' 123abc' ---
string(20) "2d363731303838363430"
--- testing: '123abc' << '123 abc' ---
string(20) "2d363731303838363430"
--- testing: '123abc' << '123abc ' ---
string(20) "2d363731303838363430"
--- testing: '123abc' << '3.4a' ---
string(6) "393834"
--- testing: '123abc' << 'a5.9' ---
string(6) "313233"
--- testing: '123e5' << '0' ---
string(6) "313233"
--- testing: '123e5' << '65' ---
string(6) "323436"
--- testing: '123e5' << '-44' ---
string(18) "313238393734383438"
--- testing: '123e5' << '1.2' ---
string(6) "323436"
--- testing: '123e5' << '-7.7' ---
string(20) "2d313637373732313630"
--- testing: '123e5' << 'abc' ---
string(6) "313233"
--- testing: '123e5' << '123abc' ---
string(20) "2d363731303838363430"
--- testing: '123e5' << '123e5' ---
string(20) "2d363731303838363430"
--- testing: '123e5' << '123e5xyz' ---
string(20) "2d363731303838363430"
--- testing: '123e5' << ' 123abc' ---
string(20) "2d363731303838363430"
--- testing: '123e5' << '123 abc' ---
string(20) "2d363731303838363430"
--- testing: '123e5' << '123abc ' ---
string(20) "2d363731303838363430"
--- testing: '123e5' << '3.4a' ---
string(6) "393834"
--- testing: '123e5' << 'a5.9' ---
string(6) "313233"
--- testing: '123e5xyz' << '0' ---
string(6) "313233"
--- testing: '123e5xyz' << '65' ---
string(6) "323436"
--- testing: '123e5xyz' << '-44' ---
string(18) "313238393734383438"
--- testing: '123e5xyz' << '1.2' ---
string(6) "323436"
--- testing: '123e5xyz' << '-7.7' ---
string(20) "2d313637373732313630"
--- testing: '123e5xyz' << 'abc' ---
string(6) "313233"
--- testing: '123e5xyz' << '123abc' ---
string(20) "2d363731303838363430"
--- testing: '123e5xyz' << '123e5' ---
string(20) "2d363731303838363430"
--- testing: '123e5xyz' << '123e5xyz' ---
string(20) "2d363731303838363430"
--- testing: '123e5xyz' << ' 123abc' ---
string(20) "2d363731303838363430"
--- testing: '123e5xyz' << '123 abc' ---
string(20) "2d363731303838363430"
--- testing: '123e5xyz' << '123abc ' ---
string(20) "2d363731303838363430"
--- testing: '123e5xyz' << '3.4a' ---
string(6) "393834"
--- testing: '123e5xyz' << 'a5.9' ---
string(6) "313233"
--- testing: ' 123abc' << '0' ---
string(6) "313233"
--- testing: ' 123abc' << '65' ---
string(6) "323436"
--- testing: ' 123abc' << '-44' ---
string(18) "313238393734383438"
--- testing: ' 123abc' << '1.2' ---
string(6) "323436"
--- testing: ' 123abc' << '-7.7' ---
string(20) "2d313637373732313630"
--- testing: ' 123abc' << 'abc' ---
string(6) "313233"
--- testing: ' 123abc' << '123abc' ---
string(20) "2d363731303838363430"
--- testing: ' 123abc' << '123e5' ---
string(20) "2d363731303838363430"
--- testing: ' 123abc' << '123e5xyz' ---
string(20) "2d363731303838363430"
--- testing: ' 123abc' << ' 123abc' ---
string(20) "2d363731303838363430"
--- testing: ' 123abc' << '123 abc' ---
string(20) "2d363731303838363430"
--- testing: ' 123abc' << '123abc ' ---
string(20) "2d363731303838363430"
--- testing: ' 123abc' << '3.4a' ---
string(6) "393834"
--- testing: ' 123abc' << 'a5.9' ---
string(6) "313233"
--- testing: '123 abc' << '0' ---
string(6) "313233"
--- testing: '123 abc' << '65' ---
string(6) "323436"
--- testing: '123 abc' << '-44' ---
string(18) "313238393734383438"
--- testing: '123 abc' << '1.2' ---
string(6) "323436"
--- testing: '123 abc' << '-7.7' ---
string(20) "2d313637373732313630"
--- testing: '123 abc' << 'abc' ---
string(6) "313233"
--- testing: '123 abc' << '123abc' ---
string(20) "2d363731303838363430"
--- testing: '123 abc' << '123e5' ---
string(20) "2d363731303838363430"
--- testing: '123 abc' << '123e5xyz' ---
string(20) "2d363731303838363430"
--- testing: '123 abc' << ' 123abc' ---
string(20) "2d363731303838363430"
--- testing: '123 abc' << '123 abc' ---
string(20) "2d363731303838363430"
--- testing: '123 abc' << '123abc ' ---
string(20) "2d363731303838363430"
--- testing: '123 abc' << '3.4a' ---
string(6) "393834"
--- testing: '123 abc' << 'a5.9' ---
string(6) "313233"
--- testing: '123abc ' << '0' ---
string(6) "313233"
--- testing: '123abc ' << '65' ---
string(6) "323436"
--- testing: '123abc ' << '-44' ---
string(18) "313238393734383438"
--- testing: '123abc ' << '1.2' ---
string(6) "323436"
--- testing: '123abc ' << '-7.7' ---
string(20) "2d313637373732313630"
--- testing: '123abc ' << 'abc' ---
string(6) "313233"
--- testing: '123abc ' << '123abc' ---
string(20) "2d363731303838363430"
--- testing: '123abc ' << '123e5' ---
string(20) "2d363731303838363430"
--- testing: '123abc ' << '123e5xyz' ---
string(20) "2d363731303838363430"
--- testing: '123abc ' << ' 123abc' ---
string(20) "2d363731303838363430"
--- testing: '123abc ' << '123 abc' ---
string(20) "2d363731303838363430"
--- testing: '123abc ' << '123abc ' ---
string(20) "2d363731303838363430"
--- testing: '123abc ' << '3.4a' ---
string(6) "393834"
--- testing: '123abc ' << 'a5.9' ---
string(6) "313233"
--- testing: '3.4a' << '0' ---
string(2) "33"
--- testing: '3.4a' << '65' ---
string(2) "36"
--- testing: '3.4a' << '-44' ---
string(14) "33313435373238"
--- testing: '3.4a' << '1.2' ---
string(2) "36"
--- testing: '3.4a' << '-7.7' ---
string(18) "313030363633323936"
--- testing: '3.4a' << 'abc' ---
string(2) "33"
--- testing: '3.4a' << '123abc' ---
string(18) "343032363533313834"
--- testing: '3.4a' << '123e5' ---
string(18) "343032363533313834"
--- testing: '3.4a' << '123e5xyz' ---
string(18) "343032363533313834"
--- testing: '3.4a' << ' 123abc' ---
string(18) "343032363533313834"
--- testing: '3.4a' << '123 abc' ---
string(18) "343032363533313834"
--- testing: '3.4a' << '123abc ' ---
string(18) "343032363533313834"
--- testing: '3.4a' << '3.4a' ---
string(4) "3234"
--- testing: '3.4a' << 'a5.9' ---
string(2) "33"
--- testing: 'a5.9' << '0' ---
string(2) "30"
--- testing: 'a5.9' << '65' ---
string(2) "30"
--- testing: 'a5.9' << '-44' ---
string(2) "30"
--- testing: 'a5.9' << '1.2' ---
string(2) "30"
--- testing: 'a5.9' << '-7.7' ---
string(2) "30"
--- testing: 'a5.9' << 'abc' ---
string(2) "30"
--- testing: 'a5.9' << '123abc' ---
string(2) "30"
--- testing: 'a5.9' << '123e5' ---
string(2) "30"
--- testing: 'a5.9' << '123e5xyz' ---
string(2) "30"
--- testing: 'a5.9' << ' 123abc' ---
string(2) "30"
--- testing: 'a5.9' << '123 abc' ---
string(2) "30"
--- testing: 'a5.9' << '123abc ' ---
string(2) "30"
--- testing: 'a5.9' << '3.4a' ---
string(2) "30"
--- testing: 'a5.9' << 'a5.9' ---
string(2) "30"
===DONE===

