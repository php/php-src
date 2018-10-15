--TEST--
Test whether a node has child nodes: hasChildNodes()
--SKIPIF--
<?php
include('skipif.inc');
?>
--FILE--
<?php

/* Create an XML document
 * with strcuture
 * <book>
 *  <title>This is the title</title>
 * </book>
 * Check for child nodes of the <book>, <title> and This is the title
 *
*/

$doc = new DOMDocument();

$root = $doc->createElement('book');
$doc->appendChild($root);

$title = $doc->createElement('title');
$root->appendChild($title);

$text = $doc->createTextNode('This is the title');
$title->appendChild($text);

echo "Root has child nodes: ";
var_dump($root->hasChildNodes());

echo "Title has child nodes: ";
var_dump($title->hasChildNodes());

echo "Text has child nodes: ";
var_dump($text->hasChildNodes());

?>
--EXPECTF--
Root has child nodes: bool(true)
Title has child nodes: bool(true)
Text has child nodes: bool(false)
