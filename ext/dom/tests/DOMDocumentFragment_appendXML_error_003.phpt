--TEST--
DOMDocumentFragment::appendXML() with unbalanced chunks.
--CREDITS--
Eric Lee Stewart <ericleestewart@gmail.com>
# TestFest Atlanta 2009-05-24
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);

$fragment = $document->createDocumentFragment();
@$fragment->appendXML('<foo>is<bar>great</foo>');
$root->appendChild($fragment);
?>
--EXPECTF--
Warning: DOMNode::appendChild(): Document Fragment is empty in %s on line %d