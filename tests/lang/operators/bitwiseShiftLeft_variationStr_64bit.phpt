--TEST--
Test << operator : various numbers as strings
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
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
unicode(2) "30"
--- testing: '0' << '65' ---
unicode(2) "30"
--- testing: '0' << '-44' ---
unicode(2) "30"
--- testing: '0' << '1.2' ---
unicode(2) "30"
--- testing: '0' << '-7.7' ---
unicode(2) "30"
--- testing: '0' << 'abc' ---
unicode(2) "30"
--- testing: '0' << '123abc' ---
unicode(2) "30"
--- testing: '0' << '123e5' ---
unicode(2) "30"
--- testing: '0' << '123e5xyz' ---
unicode(2) "30"
--- testing: '0' << ' 123abc' ---
unicode(2) "30"
--- testing: '0' << '123 abc' ---
unicode(2) "30"
--- testing: '0' << '123abc ' ---
unicode(2) "30"
--- testing: '0' << '3.4a' ---
unicode(2) "30"
--- testing: '0' << 'a5.9' ---
unicode(2) "30"
--- testing: '65' << '0' ---
unicode(4) "3635"
--- testing: '65' << '65' ---
unicode(6) "313330"
--- testing: '65' << '-44' ---
unicode(16) "3638313537343430"
--- testing: '65' << '1.2' ---
unicode(6) "313330"
--- testing: '65' << '-7.7' ---
unicode(40) "2d39303739323536383438373738393139393336"
--- testing: '65' << 'abc' ---
unicode(4) "3635"
--- testing: '65' << '123abc' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '65' << '123e5' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '65' << '123e5xyz' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '65' << ' 123abc' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '65' << '123 abc' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '65' << '123abc ' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '65' << '3.4a' ---
unicode(6) "353230"
--- testing: '65' << 'a5.9' ---
unicode(4) "3635"
--- testing: '-44' << '0' ---
unicode(6) "2d3434"
--- testing: '-44' << '65' ---
unicode(6) "2d3838"
--- testing: '-44' << '-44' ---
unicode(18) "2d3436313337333434"
--- testing: '-44' << '1.2' ---
unicode(6) "2d3838"
--- testing: '-44' << '-7.7' ---
unicode(40) "2d36333431303638323735333337363538333638"
--- testing: '-44' << 'abc' ---
unicode(6) "2d3434"
--- testing: '-44' << '123abc' ---
unicode(40) "2d36393137353239303237363431303831383536"
--- testing: '-44' << '123e5' ---
unicode(40) "2d36393137353239303237363431303831383536"
--- testing: '-44' << '123e5xyz' ---
unicode(40) "2d36393137353239303237363431303831383536"
--- testing: '-44' << ' 123abc' ---
unicode(40) "2d36393137353239303237363431303831383536"
--- testing: '-44' << '123 abc' ---
unicode(40) "2d36393137353239303237363431303831383536"
--- testing: '-44' << '123abc ' ---
unicode(40) "2d36393137353239303237363431303831383536"
--- testing: '-44' << '3.4a' ---
unicode(8) "2d333532"
--- testing: '-44' << 'a5.9' ---
unicode(6) "2d3434"
--- testing: '1.2' << '0' ---
unicode(2) "31"
--- testing: '1.2' << '65' ---
unicode(2) "32"
--- testing: '1.2' << '-44' ---
unicode(14) "31303438353736"
--- testing: '1.2' << '1.2' ---
unicode(2) "32"
--- testing: '1.2' << '-7.7' ---
unicode(36) "313434313135313838303735383535383732"
--- testing: '1.2' << 'abc' ---
unicode(2) "31"
--- testing: '1.2' << '123abc' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '1.2' << '123e5' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '1.2' << '123e5xyz' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '1.2' << ' 123abc' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '1.2' << '123 abc' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '1.2' << '123abc ' ---
unicode(36) "353736343630373532333033343233343838"
--- testing: '1.2' << '3.4a' ---
unicode(2) "38"
--- testing: '1.2' << 'a5.9' ---
unicode(2) "31"
--- testing: '-7.7' << '0' ---
unicode(4) "2d37"
--- testing: '-7.7' << '65' ---
unicode(6) "2d3134"
--- testing: '-7.7' << '-44' ---
unicode(16) "2d37333430303332"
--- testing: '-7.7' << '1.2' ---
unicode(6) "2d3134"
--- testing: '-7.7' << '-7.7' ---
unicode(40) "2d31303038383036333136353330393931313034"
--- testing: '-7.7' << 'abc' ---
unicode(4) "2d37"
--- testing: '-7.7' << '123abc' ---
unicode(40) "2d34303335323235323636313233393634343136"
--- testing: '-7.7' << '123e5' ---
unicode(40) "2d34303335323235323636313233393634343136"
--- testing: '-7.7' << '123e5xyz' ---
unicode(40) "2d34303335323235323636313233393634343136"
--- testing: '-7.7' << ' 123abc' ---
unicode(40) "2d34303335323235323636313233393634343136"
--- testing: '-7.7' << '123 abc' ---
unicode(40) "2d34303335323235323636313233393634343136"
--- testing: '-7.7' << '123abc ' ---
unicode(40) "2d34303335323235323636313233393634343136"
--- testing: '-7.7' << '3.4a' ---
unicode(6) "2d3536"
--- testing: '-7.7' << 'a5.9' ---
unicode(4) "2d37"
--- testing: 'abc' << '0' ---
unicode(2) "30"
--- testing: 'abc' << '65' ---
unicode(2) "30"
--- testing: 'abc' << '-44' ---
unicode(2) "30"
--- testing: 'abc' << '1.2' ---
unicode(2) "30"
--- testing: 'abc' << '-7.7' ---
unicode(2) "30"
--- testing: 'abc' << 'abc' ---
unicode(2) "30"
--- testing: 'abc' << '123abc' ---
unicode(2) "30"
--- testing: 'abc' << '123e5' ---
unicode(2) "30"
--- testing: 'abc' << '123e5xyz' ---
unicode(2) "30"
--- testing: 'abc' << ' 123abc' ---
unicode(2) "30"
--- testing: 'abc' << '123 abc' ---
unicode(2) "30"
--- testing: 'abc' << '123abc ' ---
unicode(2) "30"
--- testing: 'abc' << '3.4a' ---
unicode(2) "30"
--- testing: 'abc' << 'a5.9' ---
unicode(2) "30"
--- testing: '123abc' << '0' ---
unicode(6) "313233"
--- testing: '123abc' << '65' ---
unicode(6) "323436"
--- testing: '123abc' << '-44' ---
unicode(18) "313238393734383438"
--- testing: '123abc' << '1.2' ---
unicode(6) "323436"
--- testing: '123abc' << '-7.7' ---
unicode(38) "2d373230353735393430333739323739333630"
--- testing: '123abc' << 'abc' ---
unicode(6) "313233"
--- testing: '123abc' << '123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc' << '123e5' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc' << '123e5xyz' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc' << ' 123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc' << '123 abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc' << '123abc ' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc' << '3.4a' ---
unicode(6) "393834"
--- testing: '123abc' << 'a5.9' ---
unicode(6) "313233"
--- testing: '123e5' << '0' ---
unicode(6) "313233"
--- testing: '123e5' << '65' ---
unicode(6) "323436"
--- testing: '123e5' << '-44' ---
unicode(18) "313238393734383438"
--- testing: '123e5' << '1.2' ---
unicode(6) "323436"
--- testing: '123e5' << '-7.7' ---
unicode(38) "2d373230353735393430333739323739333630"
--- testing: '123e5' << 'abc' ---
unicode(6) "313233"
--- testing: '123e5' << '123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5' << '123e5' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5' << '123e5xyz' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5' << ' 123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5' << '123 abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5' << '123abc ' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5' << '3.4a' ---
unicode(6) "393834"
--- testing: '123e5' << 'a5.9' ---
unicode(6) "313233"
--- testing: '123e5xyz' << '0' ---
unicode(6) "313233"
--- testing: '123e5xyz' << '65' ---
unicode(6) "323436"
--- testing: '123e5xyz' << '-44' ---
unicode(18) "313238393734383438"
--- testing: '123e5xyz' << '1.2' ---
unicode(6) "323436"
--- testing: '123e5xyz' << '-7.7' ---
unicode(38) "2d373230353735393430333739323739333630"
--- testing: '123e5xyz' << 'abc' ---
unicode(6) "313233"
--- testing: '123e5xyz' << '123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5xyz' << '123e5' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5xyz' << '123e5xyz' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5xyz' << ' 123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5xyz' << '123 abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5xyz' << '123abc ' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123e5xyz' << '3.4a' ---
unicode(6) "393834"
--- testing: '123e5xyz' << 'a5.9' ---
unicode(6) "313233"
--- testing: ' 123abc' << '0' ---
unicode(6) "313233"
--- testing: ' 123abc' << '65' ---
unicode(6) "323436"
--- testing: ' 123abc' << '-44' ---
unicode(18) "313238393734383438"
--- testing: ' 123abc' << '1.2' ---
unicode(6) "323436"
--- testing: ' 123abc' << '-7.7' ---
unicode(38) "2d373230353735393430333739323739333630"
--- testing: ' 123abc' << 'abc' ---
unicode(6) "313233"
--- testing: ' 123abc' << '123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: ' 123abc' << '123e5' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: ' 123abc' << '123e5xyz' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: ' 123abc' << ' 123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: ' 123abc' << '123 abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: ' 123abc' << '123abc ' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: ' 123abc' << '3.4a' ---
unicode(6) "393834"
--- testing: ' 123abc' << 'a5.9' ---
unicode(6) "313233"
--- testing: '123 abc' << '0' ---
unicode(6) "313233"
--- testing: '123 abc' << '65' ---
unicode(6) "323436"
--- testing: '123 abc' << '-44' ---
unicode(18) "313238393734383438"
--- testing: '123 abc' << '1.2' ---
unicode(6) "323436"
--- testing: '123 abc' << '-7.7' ---
unicode(38) "2d373230353735393430333739323739333630"
--- testing: '123 abc' << 'abc' ---
unicode(6) "313233"
--- testing: '123 abc' << '123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123 abc' << '123e5' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123 abc' << '123e5xyz' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123 abc' << ' 123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123 abc' << '123 abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123 abc' << '123abc ' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123 abc' << '3.4a' ---
unicode(6) "393834"
--- testing: '123 abc' << 'a5.9' ---
unicode(6) "313233"
--- testing: '123abc ' << '0' ---
unicode(6) "313233"
--- testing: '123abc ' << '65' ---
unicode(6) "323436"
--- testing: '123abc ' << '-44' ---
unicode(18) "313238393734383438"
--- testing: '123abc ' << '1.2' ---
unicode(6) "323436"
--- testing: '123abc ' << '-7.7' ---
unicode(38) "2d373230353735393430333739323739333630"
--- testing: '123abc ' << 'abc' ---
unicode(6) "313233"
--- testing: '123abc ' << '123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc ' << '123e5' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc ' << '123e5xyz' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc ' << ' 123abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc ' << '123 abc' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc ' << '123abc ' ---
unicode(40) "2d32383832333033373631353137313137343430"
--- testing: '123abc ' << '3.4a' ---
unicode(6) "393834"
--- testing: '123abc ' << 'a5.9' ---
unicode(6) "313233"
--- testing: '3.4a' << '0' ---
unicode(2) "33"
--- testing: '3.4a' << '65' ---
unicode(2) "36"
--- testing: '3.4a' << '-44' ---
unicode(14) "33313435373238"
--- testing: '3.4a' << '1.2' ---
unicode(2) "36"
--- testing: '3.4a' << '-7.7' ---
unicode(36) "343332333435353634323237353637363136"
--- testing: '3.4a' << 'abc' ---
unicode(2) "33"
--- testing: '3.4a' << '123abc' ---
unicode(38) "31373239333832323536393130323730343634"
--- testing: '3.4a' << '123e5' ---
unicode(38) "31373239333832323536393130323730343634"
--- testing: '3.4a' << '123e5xyz' ---
unicode(38) "31373239333832323536393130323730343634"
--- testing: '3.4a' << ' 123abc' ---
unicode(38) "31373239333832323536393130323730343634"
--- testing: '3.4a' << '123 abc' ---
unicode(38) "31373239333832323536393130323730343634"
--- testing: '3.4a' << '123abc ' ---
unicode(38) "31373239333832323536393130323730343634"
--- testing: '3.4a' << '3.4a' ---
unicode(4) "3234"
--- testing: '3.4a' << 'a5.9' ---
unicode(2) "33"
--- testing: 'a5.9' << '0' ---
unicode(2) "30"
--- testing: 'a5.9' << '65' ---
unicode(2) "30"
--- testing: 'a5.9' << '-44' ---
unicode(2) "30"
--- testing: 'a5.9' << '1.2' ---
unicode(2) "30"
--- testing: 'a5.9' << '-7.7' ---
unicode(2) "30"
--- testing: 'a5.9' << 'abc' ---
unicode(2) "30"
--- testing: 'a5.9' << '123abc' ---
unicode(2) "30"
--- testing: 'a5.9' << '123e5' ---
unicode(2) "30"
--- testing: 'a5.9' << '123e5xyz' ---
unicode(2) "30"
--- testing: 'a5.9' << ' 123abc' ---
unicode(2) "30"
--- testing: 'a5.9' << '123 abc' ---
unicode(2) "30"
--- testing: 'a5.9' << '123abc ' ---
unicode(2) "30"
--- testing: 'a5.9' << '3.4a' ---
unicode(2) "30"
--- testing: 'a5.9' << 'a5.9' ---
unicode(2) "30"
===DONE===
