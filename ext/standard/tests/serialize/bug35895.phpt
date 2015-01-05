--TEST--
Bug #35895 (__sleep and private property)
--FILE--
<?php
class Parents {
   private $parents;
   public function __sleep() {
       return array("parents");
   }
}

class Child extends Parents {
    private $child;
    public function __sleep() {
        return array_merge(array("child"), parent::__sleep());
    }
}

$obj = new Child();
serialize($obj);

?>
--EXPECTF--
Notice: serialize(): "parents" returned as member variable from __sleep() but does not exist in %sbug35895.php on line %d
