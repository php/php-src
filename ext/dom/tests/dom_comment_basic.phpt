--TEST--
DOM Comment : Basic Functionality
--EXTENSIONS--
dom
--FILE--
<?php

$xml = <<< EOXML
<?xml version="1.0" encoding="ISO-8859-1"?>
<courses>
    <!-- Hello World! -->
</courses>
EOXML;

$dom = new DOMDocument();
$dom->loadXML($xml);
$root = $dom->documentElement;
var_dump($root->hasChildNodes());
$children = $root->childNodes;

for ($index = 0; $index < $children->length; $index++) {
    echo "--- child $index ---\n";
    $current = $children->item($index);
    echo get_class($current), "\n";
    var_dump($current->textContent);
}
?>
--EXPECT--
bool(true)
--- child 0 ---
DOMText
string(5) "
    "
--- child 1 ---
DOMComment
string(14) " Hello World! "
--- child 2 ---
DOMText
string(1) "
"
