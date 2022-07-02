--TEST--
Bug #66783 (UAF when appending DOMDocument to element)
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DomDocument;
$doc->loadXML('<root></root>');
$e = $doc->createElement('e');
try {
    $e->appendChild($doc);
} catch (DOMException $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Hierarchy Request Error
