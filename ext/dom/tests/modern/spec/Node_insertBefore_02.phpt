--TEST--
DOM\Node::insertBefore() with DocumentFragment and a document element
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString('<!DOCTYPE html><html><head></head><body></body></html>');

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement('a'));

try {
    $dom->insertBefore($fragment, $dom->documentElement);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $dom->saveHTML(), "\n";

?>
--EXPECT--
Cannot have more than one element child in a document
<!DOCTYPE html><html><head></head><body></body></html>
