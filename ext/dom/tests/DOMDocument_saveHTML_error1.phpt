--TEST--
DOMDocument::saveHTML() should fail if a parameter is given
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
echo $doc->saveHTML(true);
?>
--EXPECTF--
Warning: DOMDocument::saveHTML() expects exactly 0 parameters, 1 given in %s on line %d
