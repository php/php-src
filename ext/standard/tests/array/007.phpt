--TEST--
Test array_diff and array_diff_assoc behaviour
--FILE--
<?php
$a = array(1,"big"=>2,3,6,3,5,3,3,3,3,3,3,3,3,3,3);
$b = array(2,2,3,3,3,3,3,3,3,3,3,3,3,3,3);
$c = array(-1,1);
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo '$c='.var_export($c,TRUE).";\n";
var_dump(array_diff($a,$b,$c));
var_dump(array_diff_assoc($a,$b,$c));
$a = array(
'a'=>2,
'b'=>'some',
'c'=>'done',
'z'=>'foo',
'f'=>5,
'fan'=>'fen',
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
'z'=>'equal'
);
$c = array(
73=>'foo',
95=>'some');
echo '$a='.var_export($a,TRUE).";\n";
echo '$b='.var_export($b,TRUE).";\n";
echo '$c='.var_export($c,TRUE).";\n";
echo "Results:\n\n";
var_dump(array_diff($a,$b,$c));
var_dump(array_diff_assoc($a,$b,$c));
?>
--EXPECT--
$a=array (
  0 => 1,
  'big' => 2,
  1 => 3,
  2 => 6,
  3 => 3,
  4 => 5,
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
);
$c=array (
  0 => -1,
  1 => 1,
);
array(2) {
  [2]=>
  int(6)
  [4]=>
  int(5)
}
array(5) {
  [0]=>
  int(1)
  ["big"]=>
  int(2)
  [1]=>
  int(3)
  [2]=>
  int(6)
  [4]=>
  int(5)
}
$a=array (
  'a' => 2,
  'b' => 'some',
  'c' => 'done',
  'z' => 'foo',
  'f' => 5,
  'fan' => 'fen',
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
);
$c=array (
  73 => 'foo',
  95 => 'some',
);
Results:

array(5) {
  ["a"]=>
  int(2)
  ["c"]=>
  string(4) "done"
  ["fan"]=>
  string(3) "fen"
  [9]=>
  int(25)
  ["want"]=>
  string(5) "wanna"
}
array(9) {
  ["a"]=>
  int(2)
  ["b"]=>
  string(4) "some"
  ["c"]=>
  string(4) "done"
  ["z"]=>
  string(3) "foo"
  ["fan"]=>
  string(3) "fen"
  [9]=>
  int(25)
  [12]=>
  int(42)
  [45]=>
  int(42)
  ["want"]=>
  string(5) "wanna"
}
