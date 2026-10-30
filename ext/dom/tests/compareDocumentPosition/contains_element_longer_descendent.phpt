--TEST--
compareDocumentPosition: contains nodes as a descendent in a longer path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <a>
        <b/>
        <c>
            <d/>
        </c>
    </a>
</container>
XML);

$xpath = new DOMXPath($dom);
$container = $xpath->query("//container")->item(0);

foreach (["a", "b", "c", "d"] as $tag) {
    echo "--- Test $tag ---\n";
    $element = $xpath->query("//$tag")->item(0);
    var_dump($container->compareDocumentPosition($element) === (DOMNode::DOCUMENT_POSITION_FOLLOWING | DOMNode::DOCUMENT_POSITION_CONTAINED_BY));
    var_dump($element->compareDocumentPosition($container) === (DOMNode::DOCUMENT_POSITION_PRECEDING | DOMNode::DOCUMENT_POSITION_CONTAINS));    
}

?>
--EXPECT--
--- Test a ---
bool(true)
bool(true)
--- Test b ---
bool(true)
bool(true)
--- Test c ---
bool(true)
bool(true)
--- Test d ---
bool(true)
bool(true)
