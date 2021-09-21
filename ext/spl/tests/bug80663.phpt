--TEST--
Bug #80663 (Recursive SplFixedArray::setSize() may cause double-free)
--FILE--
<?php
class InvalidDestructor {
    public function __destruct() {
        $GLOBALS['obj']->setSize(0);
    }
}

$obj = new SplFixedArray(1000);
$obj[0] = new InvalidDestructor();
$obj->setSize(0);
?>
--EXPECTF--
Notice: Undefined index: obj in %s on line %d

Fatal error: Uncaught Error: Call to a member function setSize() on null in %s:%d
Stack trace:
#0 [internal function]: InvalidDestructor->__destruct()
#1 {main}
  thrown in %s on line %d
