--TEST--
GH-16777 (Calling the constructor again on a DOM object after it is in a document causes UAF)
--EXTENSIONS--
dom
--FILE--
<?php
$el = new DOMElement('name');
$el->append($child = new DOMElement('child'));
$doc = new DOMDocument();
$doc->appendChild($el);
$el->__construct('newname');
$doc->appendChild($el);
echo $doc->saveXML();
$dom2 = new DOMDocument();
try {
    $dom2->appendChild($el);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
var_dump($child->ownerDocument === $doc);
?>
--EXPECT--
<?xml version="1.0"?>
<name><child/></name>
<newname/>
Wrong Document Error
bool(true)
