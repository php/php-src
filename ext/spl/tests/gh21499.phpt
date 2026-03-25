--TEST--
GH-21499: RecursiveArrayIterator getChildren UAF after parent free
--ENV--
USE_ZEND_ALLOC=0
--FILE--
<?php
$parent = new RecursiveArrayIterator([0 => [1, 2, 3, 4, 5]]);
$child = $parent->getChildren();
unset($parent);
$arr = [10, 20, 30];
$ref = &$arr;
$child->__construct($arr);
echo "ok\n";
?>
--EXPECT--
ok
