--TEST--
Create CDATA section and change it using DOMcreateCDATASection
--CREDITS--
Nic Rosental nicrosental@gmail.com
# TestFest Atlanta 2009-5-28 
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$cdata = $document->createCDATASection('t');
$root->appendChild($cdata);
print $document->saveXML()."\n";

$cdata->data = 100;
print $document->saveXML()."\n";

?>
--EXPECT--
<?xml version="1.0"?>
<root><![CDATA[t]]></root>

<?xml version="1.0"?>
<root><![CDATA[100]]></root>
