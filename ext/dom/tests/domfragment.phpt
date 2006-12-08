--TEST--
DOMDocumentFragment Tests
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
require_once("dom_test.inc");

$dom = new DOMDocument;
$dom->loadXML(b'<root/>');
if(!$dom) {
  echo "Error while parsing the document\n";
  exit;
}

$node = $dom->documentElement;

try {
	$frag1 = new DOMDocumentFragment();
	$frag1->appendXML('<fragnode>1</fragnode>');
	$node->appendXML($frag1);
} catch (DOMException $e) {
	print "Read Only!\n\n";
}

$frag2 = $dom->createDocumentFragment();
$frag2->appendXML('<fragnode2>2</fragnode2>');
$node->appendChild($frag2);

print $dom->saveXML();
?>
--EXPECT--
Read Only!

<?xml version="1.0"?>
<root><fragnode2>2</fragnode2></root>
