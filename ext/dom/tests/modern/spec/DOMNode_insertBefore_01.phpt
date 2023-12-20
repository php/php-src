--TEST--
DOMNode::insertBefore() with DocumentFragment and dtd
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><head></head><body></body></html>');
$dom->documentElement->remove();

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement('a'));

try {
    $dom->insertBefore($fragment, $dom->doctype);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $dom->saveHTML(), "\n";

?>
--EXPECT--
Document types must be the first child in a document
<!DOCTYPE html>
