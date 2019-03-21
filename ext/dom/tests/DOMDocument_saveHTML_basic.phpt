--TEST--
DOMDocument::saveHTML() should dump the internal document into a string using HTML formatting
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once __DIR__ .'/skipif.inc';
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
echo $doc->saveHTML();
?>
--EXPECT--
<html><head><title>This is the title</title></head></html>
