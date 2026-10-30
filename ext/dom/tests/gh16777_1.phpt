--TEST--
GH-16777 (Calling the constructor again on a DOM object after it is in a document causes UAF)
--EXTENSIONS--
dom
--FILE--
<?php
$text = new DOMText('my value');
$doc = new DOMDocument();
$doc->appendChild($text);
$text->__construct('my new value');
$doc->appendChild($text);
echo $doc->saveXML();
$dom2 = new DOMDocument();
try {
    $dom2->appendChild($text);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
<?xml version="1.0"?>
my value
my new value
Wrong Document Error
