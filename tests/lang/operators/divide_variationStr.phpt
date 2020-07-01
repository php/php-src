--TEST--
Test / operator : various numbers as strings
--FILE--
<?php

$strVals = array(
   "0","65","-44", "1.2", "-7.7", "abc", "123abc", "123e5", "123e5xyz", " 123abc", "123 abc", "123abc ", "3.4a",
   "a5.9"
);

error_reporting(E_ERROR);

foreach ($strVals as $strVal) {
   foreach($strVals as $otherVal) {
       echo "--- testing: '$strVal' / '$otherVal' ---\n";
      var_dump($strVal/$otherVal);
   }
}


?>
--EXPECT--
--- testing: '0' / '0' ---
float(NAN)
--- testing: '0' / '65' ---
int(0)
--- testing: '0' / '-44' ---
int(0)
--- testing: '0' / '1.2' ---
float(0)
--- testing: '0' / '-7.7' ---
float(-0)
--- testing: '0' / 'abc' ---
float(NAN)
--- testing: '0' / '123abc' ---
float(NAN)
--- testing: '0' / '123e5' ---
float(0)
--- testing: '0' / '123e5xyz' ---
float(NAN)
--- testing: '0' / ' 123abc' ---
float(NAN)
--- testing: '0' / '123 abc' ---
float(NAN)
--- testing: '0' / '123abc ' ---
float(NAN)
--- testing: '0' / '3.4a' ---
float(NAN)
--- testing: '0' / 'a5.9' ---
float(NAN)
--- testing: '65' / '0' ---
float(INF)
--- testing: '65' / '65' ---
int(1)
--- testing: '65' / '-44' ---
float(-1.4772727272727273)
--- testing: '65' / '1.2' ---
float(54.16666666666667)
--- testing: '65' / '-7.7' ---
float(-8.441558441558442)
--- testing: '65' / 'abc' ---
float(INF)
--- testing: '65' / '123abc' ---
float(INF)
--- testing: '65' / '123e5' ---
float(5.2845528455284555E-6)
--- testing: '65' / '123e5xyz' ---
float(INF)
--- testing: '65' / ' 123abc' ---
float(INF)
--- testing: '65' / '123 abc' ---
float(INF)
--- testing: '65' / '123abc ' ---
float(INF)
--- testing: '65' / '3.4a' ---
float(INF)
--- testing: '65' / 'a5.9' ---
float(INF)
--- testing: '-44' / '0' ---
float(-INF)
--- testing: '-44' / '65' ---
float(-0.676923076923077)
--- testing: '-44' / '-44' ---
int(1)
--- testing: '-44' / '1.2' ---
float(-36.66666666666667)
--- testing: '-44' / '-7.7' ---
float(5.714285714285714)
--- testing: '-44' / 'abc' ---
float(-INF)
--- testing: '-44' / '123abc' ---
float(-INF)
--- testing: '-44' / '123e5' ---
float(-3.5772357723577236E-6)
--- testing: '-44' / '123e5xyz' ---
float(-INF)
--- testing: '-44' / ' 123abc' ---
float(-INF)
--- testing: '-44' / '123 abc' ---
float(-INF)
--- testing: '-44' / '123abc ' ---
float(-INF)
--- testing: '-44' / '3.4a' ---
float(-INF)
--- testing: '-44' / 'a5.9' ---
float(-INF)
--- testing: '1.2' / '0' ---
float(INF)
--- testing: '1.2' / '65' ---
float(0.01846153846153846)
--- testing: '1.2' / '-44' ---
float(-0.02727272727272727)
--- testing: '1.2' / '1.2' ---
float(1)
--- testing: '1.2' / '-7.7' ---
float(-0.15584415584415584)
--- testing: '1.2' / 'abc' ---
float(INF)
--- testing: '1.2' / '123abc' ---
float(INF)
--- testing: '1.2' / '123e5' ---
float(9.75609756097561E-8)
--- testing: '1.2' / '123e5xyz' ---
float(INF)
--- testing: '1.2' / ' 123abc' ---
float(INF)
--- testing: '1.2' / '123 abc' ---
float(INF)
--- testing: '1.2' / '123abc ' ---
float(INF)
--- testing: '1.2' / '3.4a' ---
float(INF)
--- testing: '1.2' / 'a5.9' ---
float(INF)
--- testing: '-7.7' / '0' ---
float(-INF)
--- testing: '-7.7' / '65' ---
float(-0.11846153846153847)
--- testing: '-7.7' / '-44' ---
float(0.17500000000000002)
--- testing: '-7.7' / '1.2' ---
float(-6.416666666666667)
--- testing: '-7.7' / '-7.7' ---
float(1)
--- testing: '-7.7' / 'abc' ---
float(-INF)
--- testing: '-7.7' / '123abc' ---
float(-INF)
--- testing: '-7.7' / '123e5' ---
float(-6.260162601626017E-7)
--- testing: '-7.7' / '123e5xyz' ---
float(-INF)
--- testing: '-7.7' / ' 123abc' ---
float(-INF)
--- testing: '-7.7' / '123 abc' ---
float(-INF)
--- testing: '-7.7' / '123abc ' ---
float(-INF)
--- testing: '-7.7' / '3.4a' ---
float(-INF)
--- testing: '-7.7' / 'a5.9' ---
float(-INF)
--- testing: 'abc' / '0' ---
float(NAN)
--- testing: 'abc' / '65' ---
int(0)
--- testing: 'abc' / '-44' ---
int(0)
--- testing: 'abc' / '1.2' ---
float(0)
--- testing: 'abc' / '-7.7' ---
float(-0)
--- testing: 'abc' / 'abc' ---
float(NAN)
--- testing: 'abc' / '123abc' ---
float(NAN)
--- testing: 'abc' / '123e5' ---
float(0)
--- testing: 'abc' / '123e5xyz' ---
float(NAN)
--- testing: 'abc' / ' 123abc' ---
float(NAN)
--- testing: 'abc' / '123 abc' ---
float(NAN)
--- testing: 'abc' / '123abc ' ---
float(NAN)
--- testing: 'abc' / '3.4a' ---
float(NAN)
--- testing: 'abc' / 'a5.9' ---
float(NAN)
--- testing: '123abc' / '0' ---
float(NAN)
--- testing: '123abc' / '65' ---
int(0)
--- testing: '123abc' / '-44' ---
int(0)
--- testing: '123abc' / '1.2' ---
float(0)
--- testing: '123abc' / '-7.7' ---
float(-0)
--- testing: '123abc' / 'abc' ---
float(NAN)
--- testing: '123abc' / '123abc' ---
float(NAN)
--- testing: '123abc' / '123e5' ---
float(0)
--- testing: '123abc' / '123e5xyz' ---
float(NAN)
--- testing: '123abc' / ' 123abc' ---
float(NAN)
--- testing: '123abc' / '123 abc' ---
float(NAN)
--- testing: '123abc' / '123abc ' ---
float(NAN)
--- testing: '123abc' / '3.4a' ---
float(NAN)
--- testing: '123abc' / 'a5.9' ---
float(NAN)
--- testing: '123e5' / '0' ---
float(INF)
--- testing: '123e5' / '65' ---
float(189230.76923076922)
--- testing: '123e5' / '-44' ---
float(-279545.45454545453)
--- testing: '123e5' / '1.2' ---
float(10250000)
--- testing: '123e5' / '-7.7' ---
float(-1597402.5974025973)
--- testing: '123e5' / 'abc' ---
float(INF)
--- testing: '123e5' / '123abc' ---
float(INF)
--- testing: '123e5' / '123e5' ---
float(1)
--- testing: '123e5' / '123e5xyz' ---
float(INF)
--- testing: '123e5' / ' 123abc' ---
float(INF)
--- testing: '123e5' / '123 abc' ---
float(INF)
--- testing: '123e5' / '123abc ' ---
float(INF)
--- testing: '123e5' / '3.4a' ---
float(INF)
--- testing: '123e5' / 'a5.9' ---
float(INF)
--- testing: '123e5xyz' / '0' ---
float(NAN)
--- testing: '123e5xyz' / '65' ---
int(0)
--- testing: '123e5xyz' / '-44' ---
int(0)
--- testing: '123e5xyz' / '1.2' ---
float(0)
--- testing: '123e5xyz' / '-7.7' ---
float(-0)
--- testing: '123e5xyz' / 'abc' ---
float(NAN)
--- testing: '123e5xyz' / '123abc' ---
float(NAN)
--- testing: '123e5xyz' / '123e5' ---
float(0)
--- testing: '123e5xyz' / '123e5xyz' ---
float(NAN)
--- testing: '123e5xyz' / ' 123abc' ---
float(NAN)
--- testing: '123e5xyz' / '123 abc' ---
float(NAN)
--- testing: '123e5xyz' / '123abc ' ---
float(NAN)
--- testing: '123e5xyz' / '3.4a' ---
float(NAN)
--- testing: '123e5xyz' / 'a5.9' ---
float(NAN)
--- testing: ' 123abc' / '0' ---
float(NAN)
--- testing: ' 123abc' / '65' ---
int(0)
--- testing: ' 123abc' / '-44' ---
int(0)
--- testing: ' 123abc' / '1.2' ---
float(0)
--- testing: ' 123abc' / '-7.7' ---
float(-0)
--- testing: ' 123abc' / 'abc' ---
float(NAN)
--- testing: ' 123abc' / '123abc' ---
float(NAN)
--- testing: ' 123abc' / '123e5' ---
float(0)
--- testing: ' 123abc' / '123e5xyz' ---
float(NAN)
--- testing: ' 123abc' / ' 123abc' ---
float(NAN)
--- testing: ' 123abc' / '123 abc' ---
float(NAN)
--- testing: ' 123abc' / '123abc ' ---
float(NAN)
--- testing: ' 123abc' / '3.4a' ---
float(NAN)
--- testing: ' 123abc' / 'a5.9' ---
float(NAN)
--- testing: '123 abc' / '0' ---
float(NAN)
--- testing: '123 abc' / '65' ---
int(0)
--- testing: '123 abc' / '-44' ---
int(0)
--- testing: '123 abc' / '1.2' ---
float(0)
--- testing: '123 abc' / '-7.7' ---
float(-0)
--- testing: '123 abc' / 'abc' ---
float(NAN)
--- testing: '123 abc' / '123abc' ---
float(NAN)
--- testing: '123 abc' / '123e5' ---
float(0)
--- testing: '123 abc' / '123e5xyz' ---
float(NAN)
--- testing: '123 abc' / ' 123abc' ---
float(NAN)
--- testing: '123 abc' / '123 abc' ---
float(NAN)
--- testing: '123 abc' / '123abc ' ---
float(NAN)
--- testing: '123 abc' / '3.4a' ---
float(NAN)
--- testing: '123 abc' / 'a5.9' ---
float(NAN)
--- testing: '123abc ' / '0' ---
float(NAN)
--- testing: '123abc ' / '65' ---
int(0)
--- testing: '123abc ' / '-44' ---
int(0)
--- testing: '123abc ' / '1.2' ---
float(0)
--- testing: '123abc ' / '-7.7' ---
float(-0)
--- testing: '123abc ' / 'abc' ---
float(NAN)
--- testing: '123abc ' / '123abc' ---
float(NAN)
--- testing: '123abc ' / '123e5' ---
float(0)
--- testing: '123abc ' / '123e5xyz' ---
float(NAN)
--- testing: '123abc ' / ' 123abc' ---
float(NAN)
--- testing: '123abc ' / '123 abc' ---
float(NAN)
--- testing: '123abc ' / '123abc ' ---
float(NAN)
--- testing: '123abc ' / '3.4a' ---
float(NAN)
--- testing: '123abc ' / 'a5.9' ---
float(NAN)
--- testing: '3.4a' / '0' ---
float(NAN)
--- testing: '3.4a' / '65' ---
int(0)
--- testing: '3.4a' / '-44' ---
int(0)
--- testing: '3.4a' / '1.2' ---
float(0)
--- testing: '3.4a' / '-7.7' ---
float(-0)
--- testing: '3.4a' / 'abc' ---
float(NAN)
--- testing: '3.4a' / '123abc' ---
float(NAN)
--- testing: '3.4a' / '123e5' ---
float(0)
--- testing: '3.4a' / '123e5xyz' ---
float(NAN)
--- testing: '3.4a' / ' 123abc' ---
float(NAN)
--- testing: '3.4a' / '123 abc' ---
float(NAN)
--- testing: '3.4a' / '123abc ' ---
float(NAN)
--- testing: '3.4a' / '3.4a' ---
float(NAN)
--- testing: '3.4a' / 'a5.9' ---
float(NAN)
--- testing: 'a5.9' / '0' ---
float(NAN)
--- testing: 'a5.9' / '65' ---
int(0)
--- testing: 'a5.9' / '-44' ---
int(0)
--- testing: 'a5.9' / '1.2' ---
float(0)
--- testing: 'a5.9' / '-7.7' ---
float(-0)
--- testing: 'a5.9' / 'abc' ---
float(NAN)
--- testing: 'a5.9' / '123abc' ---
float(NAN)
--- testing: 'a5.9' / '123e5' ---
float(0)
--- testing: 'a5.9' / '123e5xyz' ---
float(NAN)
--- testing: 'a5.9' / ' 123abc' ---
float(NAN)
--- testing: 'a5.9' / '123 abc' ---
float(NAN)
--- testing: 'a5.9' / '123abc ' ---
float(NAN)
--- testing: 'a5.9' / '3.4a' ---
float(NAN)
--- testing: 'a5.9' / 'a5.9' ---
float(NAN)
