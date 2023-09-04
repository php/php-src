--TEST--
compareDocumentPosition: attribute order for same element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container a="b" c="d" e="f" />
XML);

$attrs = $dom->documentElement->attributes;
for ($i = 0; $i <= 2; $i++) {
    for ($j = $i + 1; $j <= $i + 2; $j++) {
        echo "--- Compare $i and ", ($j % 3), " ---\n";
        var_dump($attrs[$i]->compareDocumentPosition($attrs[$j % 3]));
    }
}

?>
--EXPECT--
--- Compare 0 and 1 ---
int(36)
--- Compare 0 and 2 ---
int(36)
--- Compare 1 and 2 ---
int(36)
--- Compare 1 and 0 ---
int(34)
--- Compare 2 and 0 ---
int(34)
--- Compare 2 and 1 ---
int(34)
