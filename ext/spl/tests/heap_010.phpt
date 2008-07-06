--TEST--
SPL: SplHeap with overriden count()
--FILE--
<?php
$obj = new SplMaxHeap();
$obj->insert(1);
$obj->insert(2);
var_dump(count($obj));
class SplMaxHeap2 extends SplMaxHeap{
    public function count() {
        return -parent::count();
    }
}
$obj = new SplMaxHeap2();
$obj->insert(1);
$obj->insert(2);
var_dump(count($obj));
?>
--EXPECT--
int(2)
int(-2)
