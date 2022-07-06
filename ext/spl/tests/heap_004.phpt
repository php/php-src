--TEST--
SPL: SplHeap: exceptions
--FILE--
<?php
class myHeap extends SplHeap {
    public function compare($a, $b) {
        throw new exception("foo");
    }
}

$h = new myHeap;

try {
    $h->insert(1);
    echo "inserted 1\n";
    $h->insert(2);
    echo "inserted 2\n";
    $h->insert(3);
    echo "inserted 3\n";
} catch(Exception $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

try {
    $h->insert(4);
    echo "inserted 4\n";
} catch(Exception $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

try {
    var_dump($h->extract());
} catch(Exception $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
try {
    var_dump($h->extract());
} catch(Exception $e) {
    echo "Exception: ".$e->getMessage()."\n";
}

echo "Recovering..\n";
$h->recoverFromCorruption();

try {
    var_dump($h->extract());
} catch(Exception $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
try {
    var_dump($h->extract());
} catch(Exception $e) {
    echo "Exception: ".$e->getMessage()."\n";
}
?>
--EXPECT--
inserted 1
Exception: foo
Exception: Heap is corrupted, heap properties are no longer ensured.
Exception: Heap is corrupted, heap properties are no longer ensured.
Exception: Heap is corrupted, heap properties are no longer ensured.
Recovering..
int(1)
int(2)
