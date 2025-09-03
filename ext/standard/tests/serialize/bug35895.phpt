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
Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Deprecated: The __sleep() serialization magic method has been deprecated. Implement __serialize() instead (or in addition, if support for old PHP versions is necessary) in %s on line %d

Warning: serialize(): "parents" returned as member variable from __sleep() but does not exist in %s on line %d
