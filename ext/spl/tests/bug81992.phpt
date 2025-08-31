--TEST--
Bug #81992 (SplFixedArray::setSize() causes use-after-free)
--FILE--
<?php
class InvalidDestructor {
    public function __destruct() {
        global $obj;
        var_dump($obj[0]);
        try {
            var_dump($obj[2]);
        } catch (Throwable $e) {
            echo $e->getMessage(), "\n";
        }
        try {
            var_dump($obj[4]);
        } catch (Throwable $e) {
            echo $e->getMessage(), "\n";
        }
    }
}

$obj = new SplFixedArray(5);
$obj[0] = str_repeat("A", 10);
$obj[2] = str_repeat('B', 10);
$obj[3] = new InvalidDestructor();
$obj[4] = str_repeat('C', 10);
$obj->setSize(2);
?>
--EXPECT--
string(10) "AAAAAAAAAA"
Index invalid or out of range
Index invalid or out of range
