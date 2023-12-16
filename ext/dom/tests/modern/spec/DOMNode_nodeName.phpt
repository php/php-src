--TEST--
DOMNode::$nodeName
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- For HTML document ---\n";

$dom = DOM\HTMLDocument::createEmpty();

$dom->appendChild($dom->createElement('container'));
var_dump($dom->createAttributeNS('http://www.w3.org/1999/xhtml', 'html')->nodeName);
var_dump($dom->createAttributeNS('http://www.w3.org/1999/xhtml', 'foo:html')->nodeName);

echo "\n";

var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'html')->nodeName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'html')->nodeName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'foo:html')->nodeName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'foo:html')->nodeName);
var_dump($dom->createElementNS('', 'html')->nodeName);
var_dump($dom->createElementNS(null, 'html')->nodeName);

echo "--- For XML document ---\n";

$dom = DOM\XMLDocument::createEmpty();

$dom->appendChild($dom->createElement('container'));
var_dump($dom->createAttributeNS('http://www.w3.org/1999/xhtml', 'html')->nodeName);
var_dump($dom->createAttributeNS('http://www.w3.org/1999/xhtml', 'foo:html')->nodeName);

echo "\n";

var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'html')->nodeName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'html')->nodeName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'foo:html')->nodeName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'foo:html')->nodeName);
var_dump($dom->createElementNS('', 'html')->nodeName);
var_dump($dom->createElementNS(null, 'html')->nodeName);
?>
--EXPECT--
--- For HTML document ---
string(4) "html"
string(8) "foo:html"

string(4) "HTML"
string(4) "html"
string(8) "FOO:HTML"
string(8) "foo:html"
string(4) "html"
string(4) "html"
--- For XML document ---
string(4) "html"
string(8) "foo:html"

string(4) "html"
string(4) "html"
string(8) "foo:html"
string(8) "foo:html"
string(4) "html"
string(4) "html"
