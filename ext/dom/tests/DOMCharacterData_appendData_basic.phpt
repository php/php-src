--TEST--
DOMCharacterData::appendData basic functionality test
--CREDITS--
Mike Sullivan <mike@regexia.com>
#TestFest 2008 (London)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$cdata = $document->createElement('cdata');
$root->appendChild($cdata);

$cdatanode = $document->createCDATASection('');
$cdata->appendChild($cdatanode);
$cdatanode->appendData('data');
echo "CDATA Length (one append): " . $cdatanode->length . "\n";

$cdatanode->appendData('><&"');
echo "CDATA Length (two appends): " . $cdatanode->length . "\n";

echo "CDATA Content: " . $cdatanode->data . "\n";

echo "\n" . $document->saveXML();

?>
--EXPECT--
CDATA Length (one append): 4
CDATA Length (two appends): 8
CDATA Content: data><&"

<?xml version="1.0"?>
<root><cdata><![CDATA[data><&"]]></cdata></root>
