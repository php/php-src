--TEST--
DOMDocument::saveHTMLFile() should fail with invalid filename
--CREDITS--
Knut Urdalen <knut@php.net>
#PHPTestFest2009 Norway 2009-06-09 \o/
--SKIPIF--
<?php
require_once __DIR__ .'/skipif.inc';
?>
--FILE--
<?php
$filename = null;
$doc = new DOMDocument('1.0');
$root = $doc->createElement('html');
$root = $doc->appendChild($root);
$head = $doc->createElement('head');
$head = $root->appendChild($head);
$title = $doc->createElement('title');
$title = $head->appendChild($title);
$text = $doc->createTextNode('This is the title');
$text = $title->appendChild($text);
try {
    $doc->saveHTMLFile($filename);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}
?>
--EXPECT--
DOMDocument::saveHTMLFile(): Argument #1 ($filename) must not be empty
