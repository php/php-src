--TEST--
Bug #60138 (GC crash with referenced array in RecursiveArrayIterator)
--FILE--
<?php
$tree = array(array("f"));
$category =& $tree[0];

$iterator = new RecursiveIteratorIterator(
    new RecursiveArrayIterator($tree),
    RecursiveIteratorIterator::SELF_FIRST
);
foreach($iterator as $file);
echo "ok\n";
?>
--EXPECT--
ok
