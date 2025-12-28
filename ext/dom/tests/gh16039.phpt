--TEST--
GH-16039 (Segmentation fault (access null pointer) in ext/dom/parentnode/tree.c)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$element = $dom->appendChild($dom->createElement('root'));
try {
    $element->prepend('x', new DOMEntity);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveXML();
$dom->strictErrorChecking = false; // Should not have influence
try {
    $element->prepend('x', new DOMEntity);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveXML();

?>
--EXPECT--
Invalid State Error
<?xml version="1.0"?>
<root/>
Invalid State Error
<?xml version="1.0"?>
<root/>
