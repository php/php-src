--TEST--
DOMCharacterData::deleteData() with offset exceeding string size.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--EXTENSIONS--
dom
--FILE--
<?php
$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$cdata = $document->createCDATASection('test');
$root->appendChild($cdata);
try {
    $cdata->deleteData(5, 1);
} catch (DOMException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Index Size Error
