--TEST--
Evaluation order during assignments.
--FILE--
<?php

// simple case with missing element
$f = array("hello","item2","bye");
list($a,,$b) = $f;
echo "A=$a B=$b\n";


// Warning: Cannot use a scalar value as an array in %s on line %d
$c[$c=1] = 1;

// i++ evaluated first, so $d[0] is 10
$d = array(0,10);
$i = 0;
$d[$i++] = $i*10;
// expected array is 10,10
var_dump($d);

// the f++++ makes f into 2, so $e 0 and 1 should both be 30
$e = array(0,0);
$f = 0;
$g1 = array(10,10);
$g2 = array(20,20);
$g3 = array(30,30);
$g = array($g1,$g2,$g3);
list($e[$f++],$e[$f++]) = $g[2];
// expect 30,30
var_dump($e);


$i1 = array(1,2);
$i2 = array(10,20);
$i3 = array(100,200);
$i4 = array(array(1000,2000),3000);
$i = array($i1,$i2,$i3,$i4);
$j = array(0,0,0);
$h = 0;
// a list of lists
list(list($j[$h++],$j[$h++]),$j[$h++]) = $i[3];
var_dump($j);


// list of lists with just variable assignments - expect 100,200,300
$k3 = array(100,200);
$k = array($k3,300);
list(list($l,$m),$n) = $k;
echo "L=$l M=$m N=$n\n";


// expect $x and $y to be null - this fails on php.net 5.2.1 (invalid opcode) - fixed in 5.2.3
list($o,$p) = 20;
echo "O=$o and P=$p\n";


// list of lists with blanks and nulls expect 10 20 40 50 60 70 80
$q1 = array(10,20,30,40);
$q2 = array(50,60);
$q3 = array($q1,$q2,null,70);
$q4 = array($q3,null,80);

list(list(list($r,$s,,$t),list($u,$v),,$w),,$x) = $q4;
echo "$r $s $t $u $v $w $x\n";


// expect y and z to be undefined
list($y,$z) = array();
echo "Y=$y,Z=$z\n";

// expect h to be defined and be 10
list($aa,$bb) = array(10);
echo "AA=$aa\n";

// expect cc and dd to be 10 and 30
list($cc,,$dd) = array(10,20,30,40);
echo "CC=$cc DD=$dd\n";

// expect the inner array to be defined
$ee = array("original array");
function f() {
  global $ee;
  $ee = array("array created in f()");
  return 1;
}
$ee["array entry created after f()"][f()] = "hello";
print_r($ee);

?>
--EXPECTF--
A=hello B=bye

Warning: Cannot use a scalar value as an array in %s on line %d
array(2) {
  [0]=>
  int(10)
  [1]=>
  int(10)
}
array(2) {
  [0]=>
  int(30)
  [1]=>
  int(30)
}
array(3) {
  [0]=>
  int(1000)
  [1]=>
  int(2000)
  [2]=>
  int(3000)
}
L=100 M=200 N=300
O= and P=
10 20 40 50 60 70 80

Notice: Undefined offset: 0 in %s on line %d

Notice: Undefined offset: 1 in %s on line %d
Y=,Z=

Notice: Undefined offset: 1 in %s on line %d
AA=10
CC=10 DD=30
Array
(
    [0] => array created in f()
    [array entry created after f()] => Array
        (
            [1] => hello
        )

)
