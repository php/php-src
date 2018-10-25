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
===DONE===
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
int(0)
--- testing: '0' / '123e5' ---
float(0)
--- testing: '0' / '123e5xyz' ---
float(0)
--- testing: '0' / ' 123abc' ---
int(0)
--- testing: '0' / '123 abc' ---
int(0)
--- testing: '0' / '123abc ' ---
int(0)
--- testing: '0' / '3.4a' ---
float(0)
--- testing: '0' / 'a5.9' ---
float(NAN)
--- testing: '65' / '0' ---
float(INF)
--- testing: '65' / '65' ---
int(1)
--- testing: '65' / '-44' ---
float(-1.4772727272727)
--- testing: '65' / '1.2' ---
float(54.166666666667)
--- testing: '65' / '-7.7' ---
float(-8.4415584415584)
--- testing: '65' / 'abc' ---
float(INF)
--- testing: '65' / '123abc' ---
float(0.52845528455285)
--- testing: '65' / '123e5' ---
float(5.2845528455285E-6)
--- testing: '65' / '123e5xyz' ---
float(5.2845528455285E-6)
--- testing: '65' / ' 123abc' ---
float(0.52845528455285)
--- testing: '65' / '123 abc' ---
float(0.52845528455285)
--- testing: '65' / '123abc ' ---
float(0.52845528455285)
--- testing: '65' / '3.4a' ---
float(19.117647058824)
--- testing: '65' / 'a5.9' ---
float(INF)
--- testing: '-44' / '0' ---
float(-INF)
--- testing: '-44' / '65' ---
float(-0.67692307692308)
--- testing: '-44' / '-44' ---
int(1)
--- testing: '-44' / '1.2' ---
float(-36.666666666667)
--- testing: '-44' / '-7.7' ---
float(5.7142857142857)
--- testing: '-44' / 'abc' ---
float(-INF)
--- testing: '-44' / '123abc' ---
float(-0.35772357723577)
--- testing: '-44' / '123e5' ---
float(-3.5772357723577E-6)
--- testing: '-44' / '123e5xyz' ---
float(-3.5772357723577E-6)
--- testing: '-44' / ' 123abc' ---
float(-0.35772357723577)
--- testing: '-44' / '123 abc' ---
float(-0.35772357723577)
--- testing: '-44' / '123abc ' ---
float(-0.35772357723577)
--- testing: '-44' / '3.4a' ---
float(-12.941176470588)
--- testing: '-44' / 'a5.9' ---
float(-INF)
--- testing: '1.2' / '0' ---
float(INF)
--- testing: '1.2' / '65' ---
float(0.018461538461538)
--- testing: '1.2' / '-44' ---
float(-0.027272727272727)
--- testing: '1.2' / '1.2' ---
float(1)
--- testing: '1.2' / '-7.7' ---
float(-0.15584415584416)
--- testing: '1.2' / 'abc' ---
float(INF)
--- testing: '1.2' / '123abc' ---
float(0.0097560975609756)
--- testing: '1.2' / '123e5' ---
float(9.7560975609756E-8)
--- testing: '1.2' / '123e5xyz' ---
float(9.7560975609756E-8)
--- testing: '1.2' / ' 123abc' ---
float(0.0097560975609756)
--- testing: '1.2' / '123 abc' ---
float(0.0097560975609756)
--- testing: '1.2' / '123abc ' ---
float(0.0097560975609756)
--- testing: '1.2' / '3.4a' ---
float(0.35294117647059)
--- testing: '1.2' / 'a5.9' ---
float(INF)
--- testing: '-7.7' / '0' ---
float(-INF)
--- testing: '-7.7' / '65' ---
float(-0.11846153846154)
--- testing: '-7.7' / '-44' ---
float(0.175)
--- testing: '-7.7' / '1.2' ---
float(-6.4166666666667)
--- testing: '-7.7' / '-7.7' ---
float(1)
--- testing: '-7.7' / 'abc' ---
float(-INF)
--- testing: '-7.7' / '123abc' ---
float(-0.06260162601626)
--- testing: '-7.7' / '123e5' ---
float(-6.260162601626E-7)
--- testing: '-7.7' / '123e5xyz' ---
float(-6.260162601626E-7)
--- testing: '-7.7' / ' 123abc' ---
float(-0.06260162601626)
--- testing: '-7.7' / '123 abc' ---
float(-0.06260162601626)
--- testing: '-7.7' / '123abc ' ---
float(-0.06260162601626)
--- testing: '-7.7' / '3.4a' ---
float(-2.2647058823529)
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
int(0)
--- testing: 'abc' / '123e5' ---
float(0)
--- testing: 'abc' / '123e5xyz' ---
float(0)
--- testing: 'abc' / ' 123abc' ---
int(0)
--- testing: 'abc' / '123 abc' ---
int(0)
--- testing: 'abc' / '123abc ' ---
int(0)
--- testing: 'abc' / '3.4a' ---
float(0)
--- testing: 'abc' / 'a5.9' ---
float(NAN)
--- testing: '123abc' / '0' ---
float(INF)
--- testing: '123abc' / '65' ---
float(1.8923076923077)
--- testing: '123abc' / '-44' ---
float(-2.7954545454545)
--- testing: '123abc' / '1.2' ---
float(102.5)
--- testing: '123abc' / '-7.7' ---
float(-15.974025974026)
--- testing: '123abc' / 'abc' ---
float(INF)
--- testing: '123abc' / '123abc' ---
int(1)
--- testing: '123abc' / '123e5' ---
float(1.0E-5)
--- testing: '123abc' / '123e5xyz' ---
float(1.0E-5)
--- testing: '123abc' / ' 123abc' ---
int(1)
--- testing: '123abc' / '123 abc' ---
int(1)
--- testing: '123abc' / '123abc ' ---
int(1)
--- testing: '123abc' / '3.4a' ---
float(36.176470588235)
--- testing: '123abc' / 'a5.9' ---
float(INF)
--- testing: '123e5' / '0' ---
float(INF)
--- testing: '123e5' / '65' ---
float(189230.76923077)
--- testing: '123e5' / '-44' ---
float(-279545.45454545)
--- testing: '123e5' / '1.2' ---
float(10250000)
--- testing: '123e5' / '-7.7' ---
float(-1597402.5974026)
--- testing: '123e5' / 'abc' ---
float(INF)
--- testing: '123e5' / '123abc' ---
float(100000)
--- testing: '123e5' / '123e5' ---
float(1)
--- testing: '123e5' / '123e5xyz' ---
float(1)
--- testing: '123e5' / ' 123abc' ---
float(100000)
--- testing: '123e5' / '123 abc' ---
float(100000)
--- testing: '123e5' / '123abc ' ---
float(100000)
--- testing: '123e5' / '3.4a' ---
float(3617647.0588235)
--- testing: '123e5' / 'a5.9' ---
float(INF)
--- testing: '123e5xyz' / '0' ---
float(INF)
--- testing: '123e5xyz' / '65' ---
float(189230.76923077)
--- testing: '123e5xyz' / '-44' ---
float(-279545.45454545)
--- testing: '123e5xyz' / '1.2' ---
float(10250000)
--- testing: '123e5xyz' / '-7.7' ---
float(-1597402.5974026)
--- testing: '123e5xyz' / 'abc' ---
float(INF)
--- testing: '123e5xyz' / '123abc' ---
float(100000)
--- testing: '123e5xyz' / '123e5' ---
float(1)
--- testing: '123e5xyz' / '123e5xyz' ---
float(1)
--- testing: '123e5xyz' / ' 123abc' ---
float(100000)
--- testing: '123e5xyz' / '123 abc' ---
float(100000)
--- testing: '123e5xyz' / '123abc ' ---
float(100000)
--- testing: '123e5xyz' / '3.4a' ---
float(3617647.0588235)
--- testing: '123e5xyz' / 'a5.9' ---
float(INF)
--- testing: ' 123abc' / '0' ---
float(INF)
--- testing: ' 123abc' / '65' ---
float(1.8923076923077)
--- testing: ' 123abc' / '-44' ---
float(-2.7954545454545)
--- testing: ' 123abc' / '1.2' ---
float(102.5)
--- testing: ' 123abc' / '-7.7' ---
float(-15.974025974026)
--- testing: ' 123abc' / 'abc' ---
float(INF)
--- testing: ' 123abc' / '123abc' ---
int(1)
--- testing: ' 123abc' / '123e5' ---
float(1.0E-5)
--- testing: ' 123abc' / '123e5xyz' ---
float(1.0E-5)
--- testing: ' 123abc' / ' 123abc' ---
int(1)
--- testing: ' 123abc' / '123 abc' ---
int(1)
--- testing: ' 123abc' / '123abc ' ---
int(1)
--- testing: ' 123abc' / '3.4a' ---
float(36.176470588235)
--- testing: ' 123abc' / 'a5.9' ---
float(INF)
--- testing: '123 abc' / '0' ---
float(INF)
--- testing: '123 abc' / '65' ---
float(1.8923076923077)
--- testing: '123 abc' / '-44' ---
float(-2.7954545454545)
--- testing: '123 abc' / '1.2' ---
float(102.5)
--- testing: '123 abc' / '-7.7' ---
float(-15.974025974026)
--- testing: '123 abc' / 'abc' ---
float(INF)
--- testing: '123 abc' / '123abc' ---
int(1)
--- testing: '123 abc' / '123e5' ---
float(1.0E-5)
--- testing: '123 abc' / '123e5xyz' ---
float(1.0E-5)
--- testing: '123 abc' / ' 123abc' ---
int(1)
--- testing: '123 abc' / '123 abc' ---
int(1)
--- testing: '123 abc' / '123abc ' ---
int(1)
--- testing: '123 abc' / '3.4a' ---
float(36.176470588235)
--- testing: '123 abc' / 'a5.9' ---
float(INF)
--- testing: '123abc ' / '0' ---
float(INF)
--- testing: '123abc ' / '65' ---
float(1.8923076923077)
--- testing: '123abc ' / '-44' ---
float(-2.7954545454545)
--- testing: '123abc ' / '1.2' ---
float(102.5)
--- testing: '123abc ' / '-7.7' ---
float(-15.974025974026)
--- testing: '123abc ' / 'abc' ---
float(INF)
--- testing: '123abc ' / '123abc' ---
int(1)
--- testing: '123abc ' / '123e5' ---
float(1.0E-5)
--- testing: '123abc ' / '123e5xyz' ---
float(1.0E-5)
--- testing: '123abc ' / ' 123abc' ---
int(1)
--- testing: '123abc ' / '123 abc' ---
int(1)
--- testing: '123abc ' / '123abc ' ---
int(1)
--- testing: '123abc ' / '3.4a' ---
float(36.176470588235)
--- testing: '123abc ' / 'a5.9' ---
float(INF)
--- testing: '3.4a' / '0' ---
float(INF)
--- testing: '3.4a' / '65' ---
float(0.052307692307692)
--- testing: '3.4a' / '-44' ---
float(-0.077272727272727)
--- testing: '3.4a' / '1.2' ---
float(2.8333333333333)
--- testing: '3.4a' / '-7.7' ---
float(-0.44155844155844)
--- testing: '3.4a' / 'abc' ---
float(INF)
--- testing: '3.4a' / '123abc' ---
float(0.027642276422764)
--- testing: '3.4a' / '123e5' ---
float(2.7642276422764E-7)
--- testing: '3.4a' / '123e5xyz' ---
float(2.7642276422764E-7)
--- testing: '3.4a' / ' 123abc' ---
float(0.027642276422764)
--- testing: '3.4a' / '123 abc' ---
float(0.027642276422764)
--- testing: '3.4a' / '123abc ' ---
float(0.027642276422764)
--- testing: '3.4a' / '3.4a' ---
float(1)
--- testing: '3.4a' / 'a5.9' ---
float(INF)
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
int(0)
--- testing: 'a5.9' / '123e5' ---
float(0)
--- testing: 'a5.9' / '123e5xyz' ---
float(0)
--- testing: 'a5.9' / ' 123abc' ---
int(0)
--- testing: 'a5.9' / '123 abc' ---
int(0)
--- testing: 'a5.9' / '123abc ' ---
int(0)
--- testing: 'a5.9' / '3.4a' ---
float(0)
--- testing: 'a5.9' / 'a5.9' ---
float(NAN)
===DONE===
