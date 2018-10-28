--TEST--
Bug #68175 (RegexIterator pregFlags are NULL instead of 0)
--FILE--
<?php
$arr = new ArrayIterator(array());
$regex = new RegexIterator($arr, '/^test/');
var_dump(
    $regex->getMode(),
    $regex->getFlags(),
    $regex->getPregFlags()
);
?>
===DONE===
--EXPECT--
int(0)
int(0)
int(0)
===DONE===
