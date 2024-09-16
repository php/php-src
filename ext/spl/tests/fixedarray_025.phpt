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
$a->y = &$y;
$a->z = new stdClass();
$x = &$a->x;
// NOTE: Creating references from dynamic properties in the backing C array
// is a bug that will be fixed in newer php minor versions.
$a->__wakeup();
var_dump($a);
?>
--EXPECTF--
object(MyFixedArray)#%d (3) {
  [0]=>
  &object(stdClass)#%d (0) {
  }
  [1]=>
  &object(ArrayObject)#%d (1) {
    ["storage":"ArrayObject":private]=>
    array(0) {
    }
  }
  [2]=>
  object(stdClass)#%d (0) {
  }
}
