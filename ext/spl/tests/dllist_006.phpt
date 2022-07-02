--TEST--
SPL: DoublyLinkedList: ArrayAccess
--FILE--
<?php
$a = new SplDoublyLinkedList();
$a->push(1);
$a->push(2);
$a->push(3);

$a[] = "foo";
$a[3] = 4;

var_dump($a[0]);
var_dump($a[1]);
var_dump($a[2]);
var_dump($a[3]);

echo "Unsetting..\n";
var_dump($a[2]);
unset($a[2]);
var_dump($a[2]);


try {
    var_dump($a["1"]);
} catch (OutOfRangeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

try {
    var_dump($a["a"]);
} catch (TypeError $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

try {
    var_dump($a["0"]);
} catch (OutOfRangeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

try {
    var_dump($a["9"]);
} catch (OutOfRangeException $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
?>
--EXPECT--
int(1)
int(2)
int(3)
int(4)
Unsetting..
int(3)
int(4)
int(2)
Exception: SplDoublyLinkedList::offsetGet(): Argument #1 ($index) must be of type int, string given
int(1)
Exception: SplDoublyLinkedList::offsetGet(): Argument #1 ($index) is out of range
