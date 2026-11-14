--TEST--
ArrayObject iterator keeps its position when the storage is separated twice between two steps
--SKIPIF--
<?php
if (getenv("USE_ZEND_ALLOC") === "0") die("skip ZMM is disabled");
?>
--FILE--
<?php
// The table the iterator is positioned on is freed between two separations, so
// the second copy is likely allocated at its address: the iterator must still
// notice that the storage changed

$ao = new ArrayObject(['a' => 1, 'b' => 2, 'c' => 3, 'd' => 4, 'e' => 5]);
foreach ($ao as $k => $v) {
    echo "$k ";
    if ($k === 'b') {
        // A hole before the current position, which a copy compacts
        unset($ao['a']);
        $data = $ao->__serialize();
        $ao['x'] = 1;
        unset($data);
        $data = $ao->__serialize();
        $ao['y'] = 1;
    }
}
echo "\n";
?>
--EXPECT--
a b c d e x y 
