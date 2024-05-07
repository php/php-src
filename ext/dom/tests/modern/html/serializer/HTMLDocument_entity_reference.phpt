--TEST--
Serialize entity reference
--EXTENSIONS--
dom
--FILE--
<?php

$dom1 = new DOMDocument();
$root = $dom1->appendChild($dom1->createElement('root'));
$root->appendChild($dom1->createEntityReference('nbsp'));

$dom2 = Dom\HTMLDocument::createEmpty();
$dom2->appendChild($dom2->importLegacyNode($root, true));

echo $dom2->saveXml(), "\n";
echo $dom2->saveHtml(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<root>&nbsp;</root>
<root>&nbsp;</root>
