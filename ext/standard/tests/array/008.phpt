--TEST--
Test array_intersect and array_intersect_assoc behaviour
--POST--
--GET--
--FILE--
<?php
//-=-=-=-=--=-=-=-=-=-=-=-=-=-=-=-=- TEST 1 -=-=-=-=-
$a = array(1,"big"=>2,2,6,3,5,3,3,454,'some_string',3,3,3,3,3,3,3,3,17);
$b = array(2,2,3,3,3,3,3,3,3,3,3,3,3,3,3,17,25,'some_string',7,8,9,109,78,17);
$c = array(-1,2,1,15,25,17);
echo str_repeat("-=",10)." TEST 1 ".str_repeat("-=",20)."\n";
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo '$c='.var_export($c,TRUE).";\n";

echo 'array_intersect($a,$b,$c);'."\n";
var_dump(array_intersect($a,$b,$c));

echo 'array_intersect_assoc($a,$b,$c);'."\n";
var_dump(array_intersect_assoc($a,$b,$c));

echo 'array_intersect($a,$b);'."\n";
var_dump(array_intersect($a,$b));

echo 'array_intersect_assoc($a,$b);'."\n";
var_dump(array_intersect_assoc($a,$b));

//-=-==-=-=-=-=-=-=-=-=-=-=-=-=-=-=- TEST 2 -=-=-=-=-=-
$a = array(
'a'=>2,
'b'=>'some',
'c'=>'done',
'z'=>'foo',
'f'=>5,
'fan'=>'fen',
'bad'=>'bed',
'gate'=>'web',
7=>18,
9=>25,
11=>42,
12=>42,
45=>42,
73=>'foo',
95=>'some',
'som3'=>'some',
'want'=>'wanna');


$b = array(
'a'=>7,
7=>18,
9=>13,
11=>42,
45=>46,
'som3'=>'some',
'foo'=>'some',
'goo'=>'foo',
'f'=>5,
'z'=>'equal',
'gate'=>'web'
);
$c = array(
'gate'=>'web',
73=>'foo',
95=>'some'
);

echo str_repeat("-=",10)." TEST 2 ".str_repeat("-=",20)."\n";
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo '$c='.var_export($c,TRUE).";\n";
echo "\n\nResults:\n\n";

echo 'array_intersect($a,$b,$c);'."\n";
var_dump(array_intersect($a,$b,$c));

echo 'array_intersect_assoc($a,$b,$c);'."\n";
var_dump(array_intersect_assoc($a,$b,$c));

echo 'array_intersect($a,$b);'."\n";
var_dump(array_intersect($a,$b));

echo 'array_intersect_assoc($a,$b);'."\n";
var_dump(array_intersect_assoc($a,$b));
?>
--EXPECT--
-=-=-=-=-=-=-=-=-=-= TEST 1 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
$a=array (
  0 => 1,
  'big' => 2,
  1 => 2,
  2 => 6,
  3 => 3,
  4 => 5,
  5 => 3,
  6 => 3,
  7 => 454,
  8 => 'some_string',
  9 => 3,
  10 => 3,
  11 => 3,
  12 => 3,
  13 => 3,
  14 => 3,
  15 => 3,
  16 => 3,
  17 => 17,
);
$b=array (
  0 => 2,
  1 => 2,
  2 => 3,
  3 => 3,
  4 => 3,
  5 => 3,
  6 => 3,
  7 => 3,
  8 => 3,
  9 => 3,
  10 => 3,
  11 => 3,
  12 => 3,
  13 => 3,
  14 => 3,
  15 => 17,
  16 => 25,
  17 => 'some_string',
  18 => 7,
  19 => 8,
  20 => 9,
  21 => 109,
  22 => 78,
  23 => 17,
);
$c=array (
  0 => -1,
  1 => 2,
  2 => 1,
  3 => 15,
  4 => 25,
  5 => 17,
);
array_intersect($a,$b,$c);
array(3) {
  ["big"]=>
  int(2)
  [1]=>
  int(2)
  [17]=>
  int(17)
}
array_intersect_assoc($a,$b,$c);
array(1) {
  [1]=>
  int(2)
}
array_intersect($a,$b);
array(15) {
  ["big"]=>
  int(2)
  [1]=>
  int(2)
  [3]=>
  int(3)
  [5]=>
  int(3)
  [6]=>
  int(3)
  [8]=>
  string(11) "some_string"
  [9]=>
  int(3)
  [10]=>
  int(3)
  [11]=>
  int(3)
  [12]=>
  int(3)
  [13]=>
  int(3)
  [14]=>
  int(3)
  [15]=>
  int(3)
  [16]=>
  int(3)
  [17]=>
  int(17)
}
array_intersect_assoc($a,$b);
array(10) {
  [1]=>
  int(2)
  [3]=>
  int(3)
  [5]=>
  int(3)
  [6]=>
  int(3)
  [9]=>
  int(3)
  [10]=>
  int(3)
  [11]=>
  int(3)
  [12]=>
  int(3)
  [13]=>
  int(3)
  [14]=>
  int(3)
}
-=-=-=-=-=-=-=-=-=-= TEST 2 -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
$a=array (
  'a' => 2,
  'b' => 'some',
  'c' => 'done',
  'z' => 'foo',
  'f' => 5,
  'fan' => 'fen',
  'bad' => 'bed',
  'gate' => 'web',
  7 => 18,
  9 => 25,
  11 => 42,
  12 => 42,
  45 => 42,
  73 => 'foo',
  95 => 'some',
  'som3' => 'some',
  'want' => 'wanna',
);
$b=array (
  'a' => 7,
  7 => 18,
  9 => 13,
  11 => 42,
  45 => 46,
  'som3' => 'some',
  'foo' => 'some',
  'goo' => 'foo',
  'f' => 5,
  'z' => 'equal',
  'gate' => 'web',
);
$c=array (
  'gate' => 'web',
  73 => 'foo',
  95 => 'some',
);


Results:

array_intersect($a,$b,$c);
array(6) {
  ["b"]=>
  string(4) "some"
  ["z"]=>
  string(3) "foo"
  ["gate"]=>
  string(3) "web"
  [73]=>
  string(3) "foo"
  [95]=>
  string(4) "some"
  ["som3"]=>
  string(4) "some"
}
array_intersect_assoc($a,$b,$c);
array(1) {
  ["gate"]=>
  string(3) "web"
}
array_intersect($a,$b);
array(11) {
  ["b"]=>
  string(4) "some"
  ["z"]=>
  string(3) "foo"
  ["f"]=>
  int(5)
  ["gate"]=>
  string(3) "web"
  [7]=>
  int(18)
  [11]=>
  int(42)
  [12]=>
  int(42)
  [45]=>
  int(42)
  [73]=>
  string(3) "foo"
  [95]=>
  string(4) "some"
  ["som3"]=>
  string(4) "some"
}
array_intersect_assoc($a,$b);
array(5) {
  ["f"]=>
  int(5)
  ["gate"]=>
  string(3) "web"
  [7]=>
  int(18)
  [11]=>
  int(42)
  ["som3"]=>
  string(4) "some"
}
