--TEST--
array_push() with several values
--FILE--
<?php
echo "Room for all values\n";
$a = range(1, 6);
var_dump(array_push($a, 7, 8), $a === range(1, 8));

echo "One value more than the room\n";
$a = range(1, 7);
var_dump(array_push($a, 8, 9), $a === range(1, 9));

echo "More values than twice the table size\n";
$a = range(1, 8);
var_dump(array_push($a, ...range(9, 25)), $a === range(1, 25));

echo "Empty array\n";
$a = [];
var_dump(array_push($a, 1, 2), $a === [1, 2]);
$a = [];
var_dump(array_push($a, ...range(0, 8)), $a === range(0, 8));

echo "No values keep the next index of an empty array\n";
$a = [];
var_dump(array_push($a));
$a[-5] = 1;
$a[] = 2;
var_dump(array_keys($a));

echo "Hole with room\n";
$a = [0, 1, 2, 3];
unset($a[1]);
var_dump(array_push($a, 4, 5), array_keys($a));

echo "Holes without room\n";
$a = range(0, 15);
for ($i = 0; $i < 14; $i++) {
    unset($a[$i]);
}
end($a);
next($a);
var_dump(array_push($a, 16, 17), key($a), array_keys($a));

echo "Next index after the last used slot\n";
$a = [0, 1, 2];
unset($a[2]);
var_dump(array_push($a, 3, 4), array_keys($a));

echo "Negative next index of an empty copy\n";
$a = [-3 => 0];
unset($a[-3]);
$copy = $a;
var_dump(array_push($a, 1, 2), array_keys($a), $copy);

echo "Internal pointer\n";
$a = [1, 2, 3];
next($a);
array_push($a, 4, 5);
var_dump(key($a));

echo "Refcounted values\n";
$o = new stdClass;
$a = [1];
array_push($a, $o, $o);
debug_zval_dump($o);
unset($a);
debug_zval_dump($o);

echo "Next index already used\n";
$a = [PHP_INT_MAX - 1 => 0];
try {
    array_push($a, $o, $o, $o);
} catch (Error $e) {
    echo $e->getMessage(), "\n";
    unset($e);
}
var_dump(array_keys($a) === [PHP_INT_MAX - 1, PHP_INT_MAX]);
debug_zval_dump($o);
?>
--EXPECT--
Room for all values
int(8)
bool(true)
One value more than the room
int(9)
bool(true)
More values than twice the table size
int(25)
bool(true)
Empty array
int(2)
bool(true)
int(9)
bool(true)
No values keep the next index of an empty array
int(0)
array(2) {
  [0]=>
  int(-5)
  [1]=>
  int(-4)
}
Hole with room
int(5)
array(5) {
  [0]=>
  int(0)
  [1]=>
  int(2)
  [2]=>
  int(3)
  [3]=>
  int(4)
  [4]=>
  int(5)
}
Holes without room
int(4)
NULL
array(4) {
  [0]=>
  int(14)
  [1]=>
  int(15)
  [2]=>
  int(16)
  [3]=>
  int(17)
}
Next index after the last used slot
int(4)
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(3)
  [3]=>
  int(4)
}
Negative next index of an empty copy
int(2)
array(2) {
  [0]=>
  int(-2)
  [1]=>
  int(-1)
}
array(0) {
}
Internal pointer
int(1)
Refcounted values
object(stdClass)#1 (0) refcount(4){
}
object(stdClass)#1 (0) refcount(2){
}
Next index already used
Cannot add element to the array as the next element is already occupied
bool(true)
object(stdClass)#1 (0) refcount(3){
}
