--TEST--
Bug #81992 (SplFixedArray::setSize() causes use-after-free) - setSize variation
--FILE--
<?php
class InvalidDestructor {
    public function __destruct() {
        global $obj;
        $obj->setSize(1);
        echo "Destroyed\n";
    }
}

$obj = new SplFixedArray(5);
$obj[0] = str_repeat("A", 10);
$obj[2] = str_repeat('B', 10);
$obj[3] = new InvalidDestructor();
$obj[4] = str_repeat('C', 10);
$obj->setSize(2);
echo "Done\n";
?>
--EXPECT--
Destroyed
Done
