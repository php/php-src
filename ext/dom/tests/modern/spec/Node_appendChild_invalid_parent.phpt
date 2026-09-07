--TEST--
Dom\Node::appendChild() invalid parent
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();

$text = $dom->createTextNode('Hello World');
$cdata = $dom->createCDATASection('Hello World');
$pi = $dom->createProcessingInstruction('Hello', '');
$attr = $dom->createAttribute('Hello');

try {
    $text->appendChild($dom->createElement('br'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $cdata->appendChild($dom->createElement('br'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $pi->appendChild($dom->createElement('br'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $attr->appendChild($dom->createElement('br'));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Hierarchy Request Error
DOMException: Hierarchy Request Error
DOMException: Hierarchy Request Error
DOMException: Hierarchy Request Error
