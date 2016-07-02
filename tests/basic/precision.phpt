--TEST--
precision setting test
--INI--
precision=14
serialize_precision=17
--FILE--
<?php
$v = array(123456789.012345678901234567890, 10/3, 987e100, 10.0000001);

echo "INI".PHP_EOL;
ini_set('precision', -1);
ini_set('serialize_precision', -1);
echo ini_get('precision'), PHP_EOL;
echo ini_get('serialize_precision'), PHP_EOL;
echo "OUTPUTS".PHP_EOL;
echo join($v, ' ').PHP_EOL;
var_dump(serialize($v));
var_export($v);echo PHP_EOL;


echo "INI".PHP_EOL;
ini_set('precision', 0);
ini_set('serialize_precision', 0);
echo ini_get('precision'), PHP_EOL;
echo ini_get('serialize_precision'), PHP_EOL;
echo "OUTPUTS".PHP_EOL;
echo join($v, ' ').PHP_EOL;
var_dump(serialize($v));
var_export($v);echo PHP_EOL;

echo "INI".PHP_EOL;
ini_set('precision', 9);
ini_set('serialize_precision', 9);
echo ini_get('precision'), PHP_EOL;
echo ini_get('serialize_precision'), PHP_EOL;
echo "OUTPUTS".PHP_EOL;
echo join($v, ' ').PHP_EOL;
var_dump(serialize($v));
var_export($v);echo PHP_EOL;

echo "INI".PHP_EOL;
ini_set('precision', 14);
ini_set('serialize_precision', 14);
echo ini_get('precision'), PHP_EOL;
echo ini_get('serialize_precision'), PHP_EOL;
echo "OUTPUTS".PHP_EOL;
echo join($v, ' ').PHP_EOL;
var_dump(serialize($v));
var_export($v);echo PHP_EOL;

echo "INI".PHP_EOL;
ini_set('precision', 17);
ini_set('serialize_precision', 17);
echo ini_get('precision'), PHP_EOL;
echo ini_get('serialize_precision'), PHP_EOL;
echo "OUTPUTS".PHP_EOL;
echo join($v, ' ').PHP_EOL;
var_dump(serialize($v));
var_export($v);echo PHP_EOL;

echo "INI".PHP_EOL;
ini_set('precision', 25);
ini_set('serialize_precision', 25);
echo ini_get('precision'), PHP_EOL;
echo ini_get('serialize_precision'), PHP_EOL;
echo "OUTPUTS".PHP_EOL;
echo join($v, ' ').PHP_EOL;
var_dump(serialize($v));
var_export($v);echo PHP_EOL;

echo "INI".PHP_EOL;
ini_set('precision', 100);
ini_set('serialize_precision', 100);
echo ini_get('precision'), PHP_EOL;
echo ini_get('serialize_precision'), PHP_EOL;
echo "OUTPUTS".PHP_EOL;
echo join($v, ' ').PHP_EOL;
var_dump(serialize($v));
var_export($v);echo PHP_EOL;
--EXPECT--
INI
-1
-1
OUTPUTS
123456789.01234567 3.3333333333333335 9.87E+102 10.0000001
string(89) "a:4:{i:0;d:123456789.01234567;i:1;d:3.3333333333333335;i:2;d:9.87E+102;i:3;d:10.0000001;}"
array (
  0 => 123456789.01234567,
  1 => 3.3333333333333335,
  2 => 9.87E+102,
  3 => 10.0000001,
)
INI
0
0
OUTPUTS
1.0E+8 3 1.0E+103 1.0E+1
string(60) "a:4:{i:0;d:1.0E+8;i:1;d:3.0E+0;i:2;d:1.0E+103;i:3;d:1.0E+1;}"
array (
  0 => 1.0E+8,
  1 => 3.0E+0,
  2 => 1.0E+103,
  3 => 1.0E+1,
)
INI
9
9
OUTPUTS
123456789 3.33333333 9.87E+102 10.0000001
string(72) "a:4:{i:0;d:123456789;i:1;d:3.33333333;i:2;d:9.87E+102;i:3;d:10.0000001;}"
array (
  0 => 123456789.0,
  1 => 3.33333333,
  2 => 9.87E+102,
  3 => 10.0000001,
)
INI
14
14
OUTPUTS
123456789.01235 3.3333333333333 9.87E+102 10.0000001
string(83) "a:4:{i:0;d:123456789.01235;i:1;d:3.3333333333333;i:2;d:9.87E+102;i:3;d:10.0000001;}"
array (
  0 => 123456789.01235,
  1 => 3.3333333333333,
  2 => 9.87E+102,
  3 => 10.0000001,
)
INI
17
17
OUTPUTS
123456789.01234567 3.3333333333333335 9.8700000000000007E+102 10.000000099999999
string(111) "a:4:{i:0;d:123456789.01234567;i:1;d:3.3333333333333335;i:2;d:9.8700000000000007E+102;i:3;d:10.000000099999999;}"
array (
  0 => 123456789.01234567,
  1 => 3.3333333333333335,
  2 => 9.8700000000000007E+102,
  3 => 10.000000099999999,
)
INI
25
25
OUTPUTS
123456789.0123456716537476 3.33333333333333348136307 9.870000000000000687310143E+102 10.0000000999999993922529
string(141) "a:4:{i:0;d:123456789.0123456716537476;i:1;d:3.33333333333333348136307;i:2;d:9.870000000000000687310143E+102;i:3;d:10.0000000999999993922529;}"
array (
  0 => 123456789.0123456716537476,
  1 => 3.33333333333333348136307,
  2 => 9.870000000000000687310143E+102,
  3 => 10.0000000999999993922529,
)
INI
100
100
OUTPUTS
123456789.01234567165374755859375 3.333333333333333481363069950020872056484222412109375 9.87000000000000068731014283095442748328521861543465424509123736073120616987695396574376473706259651E+102 10.0000000999999993922529029077850282192230224609375
string(277) "a:4:{i:0;d:123456789.01234567165374755859375;i:1;d:3.333333333333333481363069950020872056484222412109375;i:2;d:9.87000000000000068731014283095442748328521861543465424509123736073120616987695396574376473706259651E+102;i:3;d:10.0000000999999993922529029077850282192230224609375;}"
array (
  0 => 123456789.01234567165374755859375,
  1 => 3.333333333333333481363069950020872056484222412109375,
  2 => 9.87000000000000068731014283095442748328521861543465424509123736073120616987695396574376473706259651E+102,
  3 => 10.0000000999999993922529029077850282192230224609375,
)
