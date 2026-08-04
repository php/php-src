--TEST--
Dom\Element::getElementsByClassName() item() random access (cold and backwards)
--EXTENSIONS--
dom
--FILE--
<?php

/* Regression: the item() lookup must return the n-th match for random access,
 * not only for a strictly ascending / foreach walk. A fresh collection whose
 * first access is item(n) exercises the uncached path, and a decreasing index
 * on the same collection exercises the cache-discard path. Non-matching and
 * nested elements verify that iteration advances to the next matching element
 * in tree order, not merely to the next sibling. */

$dom = Dom\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<body>
    <span class="x" id="E0"></span>
    <div class="y" id="skip1"><span class="x" id="E1"></span></div>
    <span class="x" id="E2"></span>
    <p class="z"><b class="x" id="E3"></b></p>
    <span class="x" id="E4"></span>
</body>
HTML);

$body = $dom->getElementsByTagName('body')->item(0);

function id(?Dom\Element $e): string {
    return $e === null ? 'NULL' : $e->id;
}

echo "-- cold random access (fresh collection per call) --\n";
foreach ([0, 1, 2, 3, 4, 5] as $i) {
    $collection = $body->getElementsByClassName('x');
    echo "item($i) = ", id($collection->item($i)), "\n";
}

echo "-- backwards seek on one collection --\n";
$collection = $body->getElementsByClassName('x');
foreach ([4, 2, 0, 3, 1] as $i) {
    echo "item($i) = ", id($collection->item($i)), "\n";
}

echo "-- item() seed then foreach --\n";
$collection = $body->getElementsByClassName('x');
$collection->item(3);
$ids = [];
foreach ($collection as $node) {
    $ids[] = $node->id;
}
echo implode(" ", $ids), "\n";

echo "-- last-element idiom --\n";
$collection = $body->getElementsByClassName('x');
echo "length = ", $collection->length, ", last = ", id($collection->item($collection->length - 1)), "\n";

echo "-- live collection after mutation --\n";
$collection = $body->getElementsByClassName('x');
echo "item(1) = ", id($collection->item(1)), "\n";
$dom->getElementById('E1')->remove();
echo "item(1) = ", id($collection->item(1)), ", length = ", $collection->length, "\n";

?>
--EXPECT--
-- cold random access (fresh collection per call) --
item(0) = E0
item(1) = E1
item(2) = E2
item(3) = E3
item(4) = E4
item(5) = NULL
-- backwards seek on one collection --
item(4) = E4
item(2) = E2
item(0) = E0
item(3) = E3
item(1) = E1
-- item() seed then foreach --
E0 E1 E2 E3 E4
-- last-element idiom --
length = 5, last = E4
-- live collection after mutation --
item(1) = E1
item(1) = E2, length = 4
