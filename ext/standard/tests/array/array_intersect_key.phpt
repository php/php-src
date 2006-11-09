--TEST--
Test of the array_intersect_key() and array_intersect_ukey()
--FILE--
<?php
$a = array(1, 6, 2, -20, 15, 1200, -2500);
$b = array(0, 7, 2, -20, 11, 1100, -2500);
$c = array(0, 6, 2, -20, 19, 1000, -2500);
$d = array(3, 8,-2, -20, 14,  900, -2600);

$a_f = array_flip($a);
$b_f = array_flip($b);
$c_f = array_flip($c);
$d_f = array_flip($d);

/* give nicer values */
foreach ($a_f as $k=> &$a_f_el) { $a_f_el =$k*2;}
foreach ($b_f as $k=> &$b_f_el) { $b_f_el =$k*2;}
foreach ($c_f as $k=> &$c_f_el) { $c_f_el =$k*2;}
foreach ($d_f as $k=> &$d_f_el) { $d_f_el =$k*2;}

var_dump(array_intersect_key($a_f, $b_f));// keys -> 2, -20, -2500
var_dump(array_intersect_ukey($a_f, $b_f, "comp_func"));// 2, 20, -2500
var_dump(array_intersect_key($a_f, $c_f));// keys -> 6, 2, -20, -2500
var_dump(array_intersect_ukey($a_f, $c_f, "comp_func"));// 6, 2, -20, -2500
var_dump(array_intersect_key($a_f, $d_f));// -20
var_dump(array_intersect_ukey($a_f, $d_f, "comp_func"));// -20

var_dump(array_intersect_key($a_f, $b_f, $c_f));// 2, -20, -2500
var_dump(array_intersect_ukey($a_f, $b_f, $c_f, "comp_func"));// 2, -20, -2500
var_dump(array_intersect_key($a_f, $b_f, $d_f));// -20
var_dump(array_intersect_ukey($a_f, $b_f, $d_f, "comp_func"));// -20

var_dump(array_intersect_key($a_f, $b_f, $c_f, $d_f));// -20
var_dump(array_intersect_ukey($a_f, $b_f, $c_f, $d_f, "comp_func"));//-20


var_dump(array_intersect_key($b_f, $c_f));// 0, 2, -20, -2500
var_dump(array_intersect_ukey($b_f, $c_f, "comp_func"));//0, 2, -20, 2500

var_dump(array_intersect_key($b_f, $d_f));// -20
var_dump(array_intersect_ukey($b_f, $d_f, "comp_func"));// -20

var_dump(array_intersect_key($b_f, $c_f, $d_f));// -20
var_dump(array_intersect_ukey($b_f, $c_f,  $d_f, "comp_func"));// -20


echo "----- Now testing array_intersect() ------- \n";
var_dump(array_intersect($a, $b_f));
var_dump(array_uintersect($a, $b, "comp_func"));
var_dump(array_intersect($a, $b, $c));
var_dump(array_uintersect($a, $b, $c, "comp_func"));
var_dump(array_intersect($a, $b, $c, $d));
var_dump(array_uintersect($a, $b, $c, $d, "comp_func"));

///////////////////////////////////////////////////////////////////////
function comp_func($a, $b) {
        if ($a === $b) return 0;
        return ($a > $b)? 1:-1;

}
?>
--EXPECTF--
array(3) {
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(3) {
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(4) {
  [6]=>
  int(12)
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(4) {
  [6]=>
  int(12)
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(1) {
  [-20]=>
  int(-40)
}
array(1) {
  [-20]=>
  int(-40)
}
array(3) {
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(3) {
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(1) {
  [-20]=>
  int(-40)
}
array(1) {
  [-20]=>
  int(-40)
}
array(1) {
  [-20]=>
  int(-40)
}
array(1) {
  [-20]=>
  int(-40)
}
array(4) {
  [0]=>
  int(0)
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(4) {
  [0]=>
  int(0)
  [2]=>
  int(4)
  [-20]=>
  int(-40)
  [-2500]=>
  &int(-5000)
}
array(1) {
  [-20]=>
  int(-40)
}
array(1) {
  [-20]=>
  int(-40)
}
array(1) {
  [-20]=>
  int(-40)
}
array(1) {
  [-20]=>
  int(-40)
}
----- Now testing array_intersect() ------- 
array(0) {
}
array(3) {
  [2]=>
  int(2)
  [3]=>
  int(-20)
  [6]=>
  int(-2500)
}
array(3) {
  [2]=>
  int(2)
  [3]=>
  int(-20)
  [6]=>
  int(-2500)
}
array(3) {
  [2]=>
  int(2)
  [3]=>
  int(-20)
  [6]=>
  int(-2500)
}
array(1) {
  [3]=>
  int(-20)
}
array(1) {
  [3]=>
  int(-20)
}
