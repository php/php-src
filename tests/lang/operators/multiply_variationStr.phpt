--TEST--
Test * operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
	   echo "--- testing: '$strVal' * '$otherVal' ---\n";
      var_dump($strVal*$otherVal);
   }
}


?>
===DONE===
--EXPECT--
--- testing: '0' * '0' ---
int(0)
--- testing: '0' * '65' ---
int(0)
--- testing: '0' * '-44' ---
int(0)
--- testing: '0' * '1.2' ---
float(0)
--- testing: '0' * '-7.7' ---
float(-0)
--- testing: '0' * 'abc' ---
int(0)
--- testing: '0' * '123abc' ---
int(0)
--- testing: '0' * '123e5' ---
float(0)
--- testing: '0' * '123e5xyz' ---
float(0)
--- testing: '0' * ' 123abc' ---
int(0)
--- testing: '0' * '123 abc' ---
int(0)
--- testing: '0' * '123abc ' ---
int(0)
--- testing: '0' * '3.4a' ---
float(0)
--- testing: '0' * 'a5.9' ---
int(0)
--- testing: '65' * '0' ---
int(0)
--- testing: '65' * '65' ---
int(4225)
--- testing: '65' * '-44' ---
int(-2860)
--- testing: '65' * '1.2' ---
float(78)
--- testing: '65' * '-7.7' ---
float(-500.5)
--- testing: '65' * 'abc' ---
int(0)
--- testing: '65' * '123abc' ---
int(7995)
--- testing: '65' * '123e5' ---
float(799500000)
--- testing: '65' * '123e5xyz' ---
float(799500000)
--- testing: '65' * ' 123abc' ---
int(7995)
--- testing: '65' * '123 abc' ---
int(7995)
--- testing: '65' * '123abc ' ---
int(7995)
--- testing: '65' * '3.4a' ---
float(221)
--- testing: '65' * 'a5.9' ---
int(0)
--- testing: '-44' * '0' ---
int(0)
--- testing: '-44' * '65' ---
int(-2860)
--- testing: '-44' * '-44' ---
int(1936)
--- testing: '-44' * '1.2' ---
float(-52.8)
--- testing: '-44' * '-7.7' ---
float(338.8)
--- testing: '-44' * 'abc' ---
int(0)
--- testing: '-44' * '123abc' ---
int(-5412)
--- testing: '-44' * '123e5' ---
float(-541200000)
--- testing: '-44' * '123e5xyz' ---
float(-541200000)
--- testing: '-44' * ' 123abc' ---
int(-5412)
--- testing: '-44' * '123 abc' ---
int(-5412)
--- testing: '-44' * '123abc ' ---
int(-5412)
--- testing: '-44' * '3.4a' ---
float(-149.6)
--- testing: '-44' * 'a5.9' ---
int(0)
--- testing: '1.2' * '0' ---
float(0)
--- testing: '1.2' * '65' ---
float(78)
--- testing: '1.2' * '-44' ---
float(-52.8)
--- testing: '1.2' * '1.2' ---
float(1.44)
--- testing: '1.2' * '-7.7' ---
float(-9.24)
--- testing: '1.2' * 'abc' ---
float(0)
--- testing: '1.2' * '123abc' ---
float(147.6)
--- testing: '1.2' * '123e5' ---
float(14760000)
--- testing: '1.2' * '123e5xyz' ---
float(14760000)
--- testing: '1.2' * ' 123abc' ---
float(147.6)
--- testing: '1.2' * '123 abc' ---
float(147.6)
--- testing: '1.2' * '123abc ' ---
float(147.6)
--- testing: '1.2' * '3.4a' ---
float(4.08)
--- testing: '1.2' * 'a5.9' ---
float(0)
--- testing: '-7.7' * '0' ---
float(-0)
--- testing: '-7.7' * '65' ---
float(-500.5)
--- testing: '-7.7' * '-44' ---
float(338.8)
--- testing: '-7.7' * '1.2' ---
float(-9.24)
--- testing: '-7.7' * '-7.7' ---
float(59.29)
--- testing: '-7.7' * 'abc' ---
float(-0)
--- testing: '-7.7' * '123abc' ---
float(-947.1)
--- testing: '-7.7' * '123e5' ---
float(-94710000)
--- testing: '-7.7' * '123e5xyz' ---
float(-94710000)
--- testing: '-7.7' * ' 123abc' ---
float(-947.1)
--- testing: '-7.7' * '123 abc' ---
float(-947.1)
--- testing: '-7.7' * '123abc ' ---
float(-947.1)
--- testing: '-7.7' * '3.4a' ---
float(-26.18)
--- testing: '-7.7' * 'a5.9' ---
float(-0)
--- testing: 'abc' * '0' ---
int(0)
--- testing: 'abc' * '65' ---
int(0)
--- testing: 'abc' * '-44' ---
int(0)
--- testing: 'abc' * '1.2' ---
float(0)
--- testing: 'abc' * '-7.7' ---
float(-0)
--- testing: 'abc' * 'abc' ---
int(0)
--- testing: 'abc' * '123abc' ---
int(0)
--- testing: 'abc' * '123e5' ---
float(0)
--- testing: 'abc' * '123e5xyz' ---
float(0)
--- testing: 'abc' * ' 123abc' ---
int(0)
--- testing: 'abc' * '123 abc' ---
int(0)
--- testing: 'abc' * '123abc ' ---
int(0)
--- testing: 'abc' * '3.4a' ---
float(0)
--- testing: 'abc' * 'a5.9' ---
int(0)
--- testing: '123abc' * '0' ---
int(0)
--- testing: '123abc' * '65' ---
int(7995)
--- testing: '123abc' * '-44' ---
int(-5412)
--- testing: '123abc' * '1.2' ---
float(147.6)
--- testing: '123abc' * '-7.7' ---
float(-947.1)
--- testing: '123abc' * 'abc' ---
int(0)
--- testing: '123abc' * '123abc' ---
int(15129)
--- testing: '123abc' * '123e5' ---
float(1512900000)
--- testing: '123abc' * '123e5xyz' ---
float(1512900000)
--- testing: '123abc' * ' 123abc' ---
int(15129)
--- testing: '123abc' * '123 abc' ---
int(15129)
--- testing: '123abc' * '123abc ' ---
int(15129)
--- testing: '123abc' * '3.4a' ---
float(418.2)
--- testing: '123abc' * 'a5.9' ---
int(0)
--- testing: '123e5' * '0' ---
float(0)
--- testing: '123e5' * '65' ---
float(799500000)
--- testing: '123e5' * '-44' ---
float(-541200000)
--- testing: '123e5' * '1.2' ---
float(14760000)
--- testing: '123e5' * '-7.7' ---
float(-94710000)
--- testing: '123e5' * 'abc' ---
float(0)
--- testing: '123e5' * '123abc' ---
float(1512900000)
--- testing: '123e5' * '123e5' ---
float(1.5129E+14)
--- testing: '123e5' * '123e5xyz' ---
float(1.5129E+14)
--- testing: '123e5' * ' 123abc' ---
float(1512900000)
--- testing: '123e5' * '123 abc' ---
float(1512900000)
--- testing: '123e5' * '123abc ' ---
float(1512900000)
--- testing: '123e5' * '3.4a' ---
float(41820000)
--- testing: '123e5' * 'a5.9' ---
float(0)
--- testing: '123e5xyz' * '0' ---
float(0)
--- testing: '123e5xyz' * '65' ---
float(799500000)
--- testing: '123e5xyz' * '-44' ---
float(-541200000)
--- testing: '123e5xyz' * '1.2' ---
float(14760000)
--- testing: '123e5xyz' * '-7.7' ---
float(-94710000)
--- testing: '123e5xyz' * 'abc' ---
float(0)
--- testing: '123e5xyz' * '123abc' ---
float(1512900000)
--- testing: '123e5xyz' * '123e5' ---
float(1.5129E+14)
--- testing: '123e5xyz' * '123e5xyz' ---
float(1.5129E+14)
--- testing: '123e5xyz' * ' 123abc' ---
float(1512900000)
--- testing: '123e5xyz' * '123 abc' ---
float(1512900000)
--- testing: '123e5xyz' * '123abc ' ---
float(1512900000)
--- testing: '123e5xyz' * '3.4a' ---
float(41820000)
--- testing: '123e5xyz' * 'a5.9' ---
float(0)
--- testing: ' 123abc' * '0' ---
int(0)
--- testing: ' 123abc' * '65' ---
int(7995)
--- testing: ' 123abc' * '-44' ---
int(-5412)
--- testing: ' 123abc' * '1.2' ---
float(147.6)
--- testing: ' 123abc' * '-7.7' ---
float(-947.1)
--- testing: ' 123abc' * 'abc' ---
int(0)
--- testing: ' 123abc' * '123abc' ---
int(15129)
--- testing: ' 123abc' * '123e5' ---
float(1512900000)
--- testing: ' 123abc' * '123e5xyz' ---
float(1512900000)
--- testing: ' 123abc' * ' 123abc' ---
int(15129)
--- testing: ' 123abc' * '123 abc' ---
int(15129)
--- testing: ' 123abc' * '123abc ' ---
int(15129)
--- testing: ' 123abc' * '3.4a' ---
float(418.2)
--- testing: ' 123abc' * 'a5.9' ---
int(0)
--- testing: '123 abc' * '0' ---
int(0)
--- testing: '123 abc' * '65' ---
int(7995)
--- testing: '123 abc' * '-44' ---
int(-5412)
--- testing: '123 abc' * '1.2' ---
float(147.6)
--- testing: '123 abc' * '-7.7' ---
float(-947.1)
--- testing: '123 abc' * 'abc' ---
int(0)
--- testing: '123 abc' * '123abc' ---
int(15129)
--- testing: '123 abc' * '123e5' ---
float(1512900000)
--- testing: '123 abc' * '123e5xyz' ---
float(1512900000)
--- testing: '123 abc' * ' 123abc' ---
int(15129)
--- testing: '123 abc' * '123 abc' ---
int(15129)
--- testing: '123 abc' * '123abc ' ---
int(15129)
--- testing: '123 abc' * '3.4a' ---
float(418.2)
--- testing: '123 abc' * 'a5.9' ---
int(0)
--- testing: '123abc ' * '0' ---
int(0)
--- testing: '123abc ' * '65' ---
int(7995)
--- testing: '123abc ' * '-44' ---
int(-5412)
--- testing: '123abc ' * '1.2' ---
float(147.6)
--- testing: '123abc ' * '-7.7' ---
float(-947.1)
--- testing: '123abc ' * 'abc' ---
int(0)
--- testing: '123abc ' * '123abc' ---
int(15129)
--- testing: '123abc ' * '123e5' ---
float(1512900000)
--- testing: '123abc ' * '123e5xyz' ---
float(1512900000)
--- testing: '123abc ' * ' 123abc' ---
int(15129)
--- testing: '123abc ' * '123 abc' ---
int(15129)
--- testing: '123abc ' * '123abc ' ---
int(15129)
--- testing: '123abc ' * '3.4a' ---
float(418.2)
--- testing: '123abc ' * 'a5.9' ---
int(0)
--- testing: '3.4a' * '0' ---
float(0)
--- testing: '3.4a' * '65' ---
float(221)
--- testing: '3.4a' * '-44' ---
float(-149.6)
--- testing: '3.4a' * '1.2' ---
float(4.08)
--- testing: '3.4a' * '-7.7' ---
float(-26.18)
--- testing: '3.4a' * 'abc' ---
float(0)
--- testing: '3.4a' * '123abc' ---
float(418.2)
--- testing: '3.4a' * '123e5' ---
float(41820000)
--- testing: '3.4a' * '123e5xyz' ---
float(41820000)
--- testing: '3.4a' * ' 123abc' ---
float(418.2)
--- testing: '3.4a' * '123 abc' ---
float(418.2)
--- testing: '3.4a' * '123abc ' ---
float(418.2)
--- testing: '3.4a' * '3.4a' ---
float(11.56)
--- testing: '3.4a' * 'a5.9' ---
float(0)
--- testing: 'a5.9' * '0' ---
int(0)
--- testing: 'a5.9' * '65' ---
int(0)
--- testing: 'a5.9' * '-44' ---
int(0)
--- testing: 'a5.9' * '1.2' ---
float(0)
--- testing: 'a5.9' * '-7.7' ---
float(-0)
--- testing: 'a5.9' * 'abc' ---
int(0)
--- testing: 'a5.9' * '123abc' ---
int(0)
--- testing: 'a5.9' * '123e5' ---
float(0)
--- testing: 'a5.9' * '123e5xyz' ---
float(0)
--- testing: 'a5.9' * ' 123abc' ---
int(0)
--- testing: 'a5.9' * '123 abc' ---
int(0)
--- testing: 'a5.9' * '123abc ' ---
int(0)
--- testing: 'a5.9' * '3.4a' ---
float(0)
--- testing: 'a5.9' * 'a5.9' ---
int(0)
===DONE===
