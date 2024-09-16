--TEST--
SPL: FixedArray: __wakeup
--FILE--
<?php
#[AllowDynamicProperties]
class MyFixedArray extends SplFixedArray {
    public stdClass $x;
    public $y;
}
$a = new MyFixedArray();
$a->x = new stdClass();
$y = new ArrayObject();
$a->y = &$a->x;
$a->z = &$a->x;

$ser = serialize($a);
// NOTE: Creating references from dynamic properties in the backing C array
// is a bug that will be fixed in newer php minor versions.
$obj = unserialize($ser);
var_dump($obj);
var_dump($obj->toArray());
var_dump(count($obj));
var_dump($obj->x ?? null);
var_dump($obj->z ?? null);
?>
--EXPECTF--
object(MyFixedArray)#%d (3) {
  [0]=>
  &object(stdClass)#%d (0) {
  }
  [1]=>
  &object(stdClass)#%d (0) {
  }
  [2]=>
  &object(stdClass)#%d (0) {
  }
}
array(3) {
  [0]=>
  &object(stdClass)#%d (0) {
  }
  [1]=>
  &object(stdClass)#%d (0) {
  }
  [2]=>
  &object(stdClass)#%d (0) {
  }
}
int(3)
object(stdClass)#%d (0) {
}
NULL