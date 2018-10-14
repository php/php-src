--TEST--
Bug #72229 (Wrong reference when serialize/unserialize an object)
--FILE--
<?php
class C1
{
    public $arr1 = array();
    public $arr2 = array();
    public function __construct()
    {
        $this->arr1[0] = $this;
        $this->arr2[0] = $this->arr1[0];
        $var1 = &$this->arr1[0];  // Set a reference...
        unset($var1);             // ... and unset it.
    }
}
$Obj1 = new C1();
$txt1 = serialize($Obj1);
$Obj2 = unserialize($txt1);
$Obj1->arr2[0] = 50;
print_r($Obj1);
$Obj2->arr2[0] = 50;
print_r($Obj2);
?>
--EXPECTF--
C1 Object
(
    [arr1] => Array
        (
            [0] => C1 Object
 *RECURSION*
        )

    [arr2] => Array
        (
            [0] => 50
        )

)
C1 Object
(
    [arr1] => Array
        (
            [0] => C1 Object
 *RECURSION*
        )

    [arr2] => Array
        (
            [0] => 50
        )

)
