--TEST--
DOMNode::appendChild() with fragment of multiple elements
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();

$fragment = $dom->createDocumentFragment();
$fragment->appendChild($dom->createElement('foo'));
$fragment->appendChild($dom->createElement('bar'));

try {
    $dom->appendChild($fragment);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $dom->saveXML($fragment), "\n";

?>
--EXPECT--
Cannot have more than one element child in a document
<foo/><bar/>
