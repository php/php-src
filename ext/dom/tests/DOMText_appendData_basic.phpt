--TEST--
DOMText::appendData basic functionality test
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

$text = $document->createElement('text');
$root->appendChild($text);

$textnode = $document->createTextNode('');
$text->appendChild($textnode);
$textnode->appendData('data');
echo "Text Length (one append): " . $textnode->length . "\n";

$textnode->appendData('><&"');
echo "Text Length (two appends): " . $textnode->length . "\n";

echo "Text Content: " . $textnode->data . "\n";

echo "\n" . $document->saveXML();

?>
===DONE===
--EXPECT--
Text Length (one append): 4
Text Length (two appends): 8
Text Content: data><&"

<?xml version="1.0"?>
<root><text>data&gt;&lt;&amp;"</text></root>
===DONE===
	
