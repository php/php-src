--TEST--
Delayed freeing processing instruction
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$pi = $doc->appendChild($doc->createElementNS('some:ns', 'container'))
    ->appendChild($doc->createProcessingInstruction('hello', 'world'));
echo $doc->saveXML(), "\n";
$pi->parentNode->remove();
echo $doc->saveXML(), "\n";
var_dump($pi->parentNode);
var_dump($pi->nodeValue);
?>
--EXPECT--
<?xml version="1.0"?>
<container xmlns="some:ns"><?hello world?></container>

<?xml version="1.0"?>

NULL
string(5) "world"
