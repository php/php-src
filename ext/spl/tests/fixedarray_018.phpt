--TEST--
SPL: FixedArray: overridden count()
--FILE--
<?php
$obj = new SplFixedArray(2);
var_dump(count($obj));
class SplFixedArray2 extends SplFixedArray {
    public function count(): int {
        return -parent::count();
    }
}
$obj = new SplFixedArray2(2);
var_dump(count($obj));
?>
--EXPECT--
int(2)
int(-2)
