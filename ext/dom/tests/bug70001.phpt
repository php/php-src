--TEST--
Bug #70001 (Assigning to DOMNode::textContent does additional entity encoding)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$element = new DOMText('<p>foo & bar</p>');
var_dump($element->textContent);
$element = (new DOMDocument())->createTextNode('<p>foo & bar</p>');
var_dump($element->textContent);
$element->textContent = ('<p>foo & bar</p>');
var_dump($element->textContent);
?>
--EXPECT--
string(16) "<p>foo & bar</p>"
string(16) "<p>foo & bar</p>"
string(16) "<p>foo & bar</p>"
