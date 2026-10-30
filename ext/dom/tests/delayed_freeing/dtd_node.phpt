--TEST--
Delayed freeing dtd node
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$dtd = $doc->implementation->createDocumentType('qualified name', 'public', 'system');
$doc = $doc->implementation->createDocument('', '', $dtd);
echo $doc->saveXML(), "\n";
unset($doc);
echo $dtd->ownerDocument->saveXML();
$dtd->ownerDocument->removeChild($dtd);
var_dump($dtd->ownerDocument->nodeName);
?>
--EXPECT--
<?xml version="1.0"?>
<!DOCTYPE qualified name PUBLIC "public" "system">

<?xml version="1.0"?>
<!DOCTYPE qualified name PUBLIC "public" "system">
string(9) "#document"
