--TEST--
Test Csv\LazyLaxCollection::createFromBuffer(): nested loops over the same collection are independent
--EXTENSIONS--
csv
--FILE--
<?php
$collection = Csv\LazyLaxCollection::createFromBuffer("a\r\nb\r\nc\r\n");
$pairs = [];
foreach ($collection as $outer) {
    foreach ($collection as $inner) {
        $pairs[] = $outer[0] . $inner[0];
    }
}
echo implode(' ', $pairs), \PHP_EOL;

/* Two iterators advanced alternately */
$it1 = $collection->getIterator();
$it2 = $collection->getIterator();
$it1->rewind();
$it2->rewind();
$it1->next();
echo json_encode([$it1->current(), $it2->current()]), \PHP_EOL;
?>
--EXPECT--
aa ab ac ba bb bc ca cb cc
[["b"],["a"]]
