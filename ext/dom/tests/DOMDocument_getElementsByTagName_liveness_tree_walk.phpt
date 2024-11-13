--TEST--
DOMDocument::getElementsByTagName() liveness tree walk
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument;
$doc->loadXML('<root><container><a><b i="1"/><b i="2"/></a><b i="3"/></container><b i="4"/></root>');

echo "-- On first child, for --\n";
$list = $doc->documentElement->firstChild->getElementsByTagName('b');
var_dump($list->length);
for ($i = 0; $i < $list->length; $i++) {
	echo $i, " ", $list->item($i)->getAttribute('i'), "\n";
}
// Try to access one beyond to check if we don't get excess elements
var_dump($list->item($i));

echo "-- On first child, foreach --\n";
foreach ($list as $item) {
	echo $item->getAttribute('i'), "\n";
}

echo "-- On document, for --\n";
$list = $doc->getElementsByTagName('b');
var_dump($list->length);
for ($i = 0; $i < $list->length; $i++) {
	echo $i, " ", $list->item($i)->getAttribute('i'), "\n";
}
// Try to access one beyond to check if we don't get excess elements
var_dump($list->item($i));

echo "-- On document, foreach --\n";
foreach ($list as $item) {
	echo $item->getAttribute('i'), "\n";
}

echo "-- On document, after caching followed by removing --\n";

$list = $doc->documentElement->firstChild->getElementsByTagName('b');
$list->item(0); // Activate item cache
$list->item(0)->remove();
$list->item(0)->remove();
$list->item(0)->remove();
var_dump($list->length);
var_dump($list->item(0));
foreach ($list as $item) {
    echo "Should not execute\n";
}

echo "-- On document, clean list after removal --\n";
$list = $doc->documentElement->firstChild->getElementsByTagName('b');
var_dump($list->length);
var_dump($list->item(0));
foreach ($list as $item) {
    echo "Should not execute\n";
}

?>
--EXPECT--
-- On first child, for --
int(3)
0 1
1 2
2 3
NULL
-- On first child, foreach --
1
2
3
-- On document, for --
int(4)
0 1
1 2
2 3
3 4
NULL
-- On document, foreach --
1
2
3
4
-- On document, after caching followed by removing --
int(0)
NULL
-- On document, clean list after removal --
int(0)
NULL
