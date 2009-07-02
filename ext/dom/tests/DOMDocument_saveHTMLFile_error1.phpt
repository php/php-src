--TEST--
DOMDocument::saveHTMLFile() should fail if no parameter is given
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$doc = new DOMDocument('1.0');
$root = $doc->createElement('html');
$root = $doc->appendChild($root);
$head = $doc->createElement('head');
$head = $root->appendChild($head);
$title = $doc->createElement('title');
$title = $head->appendChild($title);
$text = $doc->createTextNode('This is the title');
$text = $title->appendChild($text);
$doc->saveHTMLFile();
?>
--EXPECTF--
Warning: DOMDocument::saveHTMLFile() expects exactly 1 parameter, 0 given in %s on line %d
