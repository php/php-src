--TEST--
Bug #80602 (Segfault when using DOMChildNode::before()) - after text merge variation
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new \DOMDocument();
$doc->loadXML('<a>foo<last/>bar</a>');
$foo = $doc->firstChild->firstChild;
$bar = $doc->firstChild->lastChild;

$foo->after($bar);

var_dump($doc->saveXML());

$foo->nodeValue = "x";

var_dump($doc->saveXML());

$bar->nodeValue = "y";

var_dump($doc->saveXML());

?>
--EXPECT--
string(43) "<?xml version="1.0"?>
<a>foobar<last/></a>
"
string(41) "<?xml version="1.0"?>
<a>xbar<last/></a>
"
string(39) "<?xml version="1.0"?>
<a>xy<last/></a>
"
