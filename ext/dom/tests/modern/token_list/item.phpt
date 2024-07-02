--TEST--
TokenList: item
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$list = $dom->documentElement->classList;

foreach (range(-1, 3) as $i) {
    var_dump($list->item($i));
}

echo "---\n";

$list->value = 'D';
var_dump($list->item(0));
var_dump($list->item(1));

?>
--EXPECT--
NULL
string(1) "A"
string(1) "B"
string(1) "C"
NULL
---
string(1) "D"
NULL
