--TEST--
Bug #81992 (SplFixedArray::setSize() causes use-after-free)
--FILE--
<?php
class InvalidDestructor {
    public function __destruct() {
        global $obj;
        var_dump($obj[2], $obj[4]);
    }
}

$obj = new SplFixedArray(5);
$obj[2] = str_repeat('B', 10);
$obj[3] = new InvalidDestructor();
$obj[4] = str_repeat('C', 10);
$obj->setSize(2);
?>
--EXPECT--
NULL
string(10) "CCCCCCCCCC"
