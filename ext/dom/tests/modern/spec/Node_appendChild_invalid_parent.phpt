--TEST--
DOM\Node::appendChild() invalid parent
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();

$text = $dom->createTextNode('Hello World');
$cdata = $dom->createCDATASection('Hello World');
$pi = $dom->createProcessingInstruction('Hello', '');
$attr = $dom->createAttribute('Hello');

try {
    $text->appendChild($dom->createElement('br'));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cdata->appendChild($dom->createElement('br'));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $pi->appendChild($dom->createElement('br'));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $attr->appendChild($dom->createElement('br'));
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
