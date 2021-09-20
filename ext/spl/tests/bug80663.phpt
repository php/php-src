--TEST--
Bug #80663 (Recursive SplFixedArray::setSize() may cause double-free)
--FILE--
<?php
class InvalidDestructor {
    public function __destruct() {
        try {
            $GLOBALS['obj']->setSize(0);
        } catch (LogicException $ex) {
            echo $ex->getMessage();
        }
    }
}

$obj = new SplFixedArray(1000);
$obj[0] = new InvalidDestructor();
$obj->setSize(0);
?>
--EXPECT--

