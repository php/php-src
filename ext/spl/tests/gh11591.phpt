--TEST--
Bug GH-11591 (Inconsistent behaviour with AppendIterator and empty generators)
--FILE--
<?php
function yieldEmpty()
{
    yield from [];
}

$iterator = new AppendIterator();
$iterator->append(yieldEmpty());

foreach ($iterator as $item) {
    var_dump($item);
}
?>
--EXPECT--
