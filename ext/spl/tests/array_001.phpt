--TEST--
SPL: ArrayObject
--FILE--
<?php

$ar = array(0=>0, 1=>1);
$ar = new ArrayObject($ar);

var_dump($ar);

$ar[2] = 2;
var_dump($ar[2]);
var_dump($ar["3"] = 3);

var_dump(array_merge((array)$ar, array(4=>4, 5=>5)));

var_dump($ar["a"] = "a");

var_dump($ar);
var_dump($ar[0]);
var_dump($ar[6]);
var_dump($ar["b"]);

unset($ar[1]);
unset($ar["3"]);
unset($ar["a"]);
unset($ar[7]);
unset($ar["c"]);
var_dump($ar);

$ar[] = '3';
$ar[] = 4;
var_dump($ar);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    [0]=>
    int(0)
    [1]=>
    int(1)
  }
}
int(2)
int(3)
array(6) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(4)
  [5]=>
  int(5)
}
string(1) "a"
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(5) {
    [0]=>
    int(0)
    [1]=>
    int(1)
    [2]=>
    int(2)
    [3]=>
    int(3)
    ["a"]=>
    string(1) "a"
  }
}
int(0)

Notice: Undefined offset: 6 in %sarray_001.php on line %d
NULL

Notice: Undefined index: b in %sarray_001.php on line %d
NULL

Notice: Undefined offset: 7 in %sarray_001.php on line %d

Notice: Undefined index: c in %sarray_001.php on line %d
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    [0]=>
    int(0)
    [2]=>
    int(2)
  }
}
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(4) {
    [0]=>
    int(0)
    [2]=>
    int(2)
    [4]=>
    string(1) "3"
    [5]=>
    int(4)
  }
}
===DONE===
