--TEST--
SPL: ArrayIterator implements ArrayAccess
--FILE--
<?php

$obj = new ArrayObject(array('1st', 1, 2=>'3rd', '4th'=>4));

var_dump($obj->getArrayCopy());

echo "===EMPTY===\n";
var_dump(empty($obj[0]));
var_dump(empty($obj[1]));
var_dump(empty($obj[2]));
var_dump(empty($obj['4th']));
var_dump(empty($obj['5th']));
var_dump(empty($obj[6]));

echo "===isset===\n";
var_dump(isset($obj[0]));
var_dump(isset($obj[1]));
var_dump(isset($obj[2]));
var_dump(isset($obj['4th']));
var_dump(isset($obj['5th']));
var_dump(isset($obj[6]));

echo "===offsetGet===\n";
var_dump($obj[0]);
var_dump($obj[1]);
var_dump($obj[2]);
var_dump($obj['4th']);
var_dump($obj['5th']);
var_dump($obj[6]);

echo "===offsetSet===\n";
echo "WRITE 1\n";
$obj[1] = 'Changed 1';
var_dump($obj[1]);
echo "WRITE 2\n";
$obj['4th'] = 'Changed 4th';
var_dump($obj['4th']);
echo "WRITE 3\n";
$obj['5th'] = 'Added 5th';
var_dump($obj['5th']);
echo "WRITE 4\n";
$obj[6] = 'Added 6';
var_dump($obj[6]);

var_dump($obj[0]);
var_dump($obj[2]);

$x = $obj[6] = 'changed 6';
var_dump($obj[6]);
var_dump($x);

echo "===unset===\n";
var_dump($obj->getArrayCopy());
unset($obj[2]);
unset($obj['4th']);
unset($obj[7]);
unset($obj['8th']);
var_dump($obj->getArrayCopy());

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
array(4) {
  [0]=>
  string(3) "1st"
  [1]=>
  int(1)
  [2]=>
  string(3) "3rd"
  ["4th"]=>
  int(4)
}
===EMPTY===
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
===isset===
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
bool(false)
===offsetGet===
string(3) "1st"
int(1)
string(3) "3rd"
int(4)

Notice: Undefined index:  5th in %sarray_010.php on line %d
NULL

Notice: Undefined offset:  6 in %sarray_010.php on line %d
NULL
===offsetSet===
WRITE 1
string(9) "Changed 1"
WRITE 2
string(11) "Changed 4th"
WRITE 3
string(9) "Added 5th"
WRITE 4
string(7) "Added 6"
string(3) "1st"
string(3) "3rd"
string(9) "changed 6"
string(9) "changed 6"
===unset===
array(6) {
  [0]=>
  string(3) "1st"
  [1]=>
  string(9) "Changed 1"
  [2]=>
  string(3) "3rd"
  ["4th"]=>
  string(11) "Changed 4th"
  ["5th"]=>
  string(9) "Added 5th"
  [6]=>
  string(9) "changed 6"
}

Notice: Undefined offset:  7 in %sarray_010.php on line %d

Notice: Undefined index:  8th in %sarray_010.php on line %d
array(4) {
  [0]=>
  string(3) "1st"
  [1]=>
  string(9) "Changed 1"
  ["5th"]=>
  string(9) "Added 5th"
  [6]=>
  string(9) "changed 6"
}
===DONE===
