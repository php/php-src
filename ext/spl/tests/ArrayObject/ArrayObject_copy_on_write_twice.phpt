--TEST--
ArrayObject iterator keeps its position when a shared storage is separated twice between two steps
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip ZMM is disabled");
?>
--FILE--
<?php
// The table the iterator is positioned on is freed between two separations, so
// the second copy is likely allocated at its address: the iterator must still
// notice that the storage changed

$source = [];
foreach (['a', 'b', 'c', 'd', 'e'] as $i => $k) {
    $source[$k] = $i + 1;
}
$ao = new ArrayObject($source);
unset($source);
foreach ($ao as $k => $v) {
    echo "$k ";
    if ($k === 'b') {
        // A hole before the current position, which a copy compacts
        unset($ao['a']);
        $copy = $ao->getArrayCopy();
        $ao['x'] = 1;
        unset($copy);
        $copy = $ao->getArrayCopy();
        $ao['y'] = 1;
    }
}
echo "\n";
?>
--EXPECT--
a b c d e x y 
