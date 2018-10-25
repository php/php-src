--TEST--
DOMDocument::save  Test basic function of save method
--SKIPIF--
<?php
require_once('skipif.inc');
?>
--FILE--
<?php
$doc = new DOMDocument('1.0');
$doc->formatOutput = true;

$root = $doc->createElement('book');

$root = $doc->appendChild($root);

$title = $doc->createElement('title');
$title = $root->appendChild($title);

$text = $doc->createTextNode('This is the title');
$text = $title->appendChild($text);

$temp_filename = dirname(__FILE__)."/DomDocument_save_basic.tmp";

echo 'Wrote: ' . $doc->save($temp_filename) . ' bytes'; // Wrote: 72 bytes
?>
--CLEAN--
<?php
	$temp_filename = dirname(__FILE__)."/DomDocument_save_basic.tmp";
	unlink($temp_filename);
?>
--EXPECT--
Wrote: 72 bytes
