--TEST--
__DOMCharacterData::substringData pull mid section of string
--CREDITS--
Nic Rosental nicrosental@gmail.com
# TestFest Atlanta 2009-5-28
--EXTENSIONS--
dom
--FILE--
<?php

$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$cdata = $document->createCDATASection('testfest');
$root->appendChild($cdata);
print $cdata->substringData(1, 6);

?>
--EXPECT--
estfes
