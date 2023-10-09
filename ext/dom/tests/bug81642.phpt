--TEST--
Bug #81642 (DOMChildNode::replaceWith() bug when replacing a node with itself)
--EXTENSIONS--
dom
--FILE--
<?php

// Replace with itself
$doc = new DOMDocument();
$doc->appendChild($target = $doc->createElement('test'));
$target->replaceWith($target);
var_dump($doc->saveXML());

// Replace with itself + another element
$doc = new DOMDocument();
$doc->appendChild($target = $doc->createElement('test'));
$target->replaceWith($target, $doc->createElement('foo'));
var_dump($doc->saveXML());

// Replace with text node
$doc = new DOMDocument();
$doc->appendChild($target = $doc->createElement('test'));
$target->replaceWith($target, 'foo');
var_dump($doc->saveXML());

// Replace with text node variant 2
$doc = new DOMDocument();
$doc->appendChild($target = $doc->createElement('test'));
$target->replaceWith('bar', $target, 'foo');
var_dump($doc->saveXML());

?>
--EXPECT--
string(30) "<?xml version="1.0"?>
<test/>
"
string(37) "<?xml version="1.0"?>
<test/>
<foo/>
"
string(34) "<?xml version="1.0"?>
<test/>
foo
"
string(38) "<?xml version="1.0"?>
bar
<test/>
foo
"
