--TEST--
DOM\Element::$tagName HTML-uppercased qualified name
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- For HTML document ---\n";

$dom = DOM\HTMLDocument::createEmpty();
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'html')->tagName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'html')->tagName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'foo:html')->tagName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'foo:html')->tagName);
var_dump($dom->createElementNS('', 'html')->tagName);
var_dump($dom->createElementNS(null, 'html')->tagName);

echo "--- For XML document ---\n";

$dom = DOM\XMLDocument::createEmpty();
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'html')->tagName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'html')->tagName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml', 'foo:html')->tagName);
var_dump($dom->createElementNS('http://www.w3.org/1999/xhtml/', 'foo:html')->tagName);
var_dump($dom->createElementNS('', 'html')->tagName);
var_dump($dom->createElementNS(null, 'html')->tagName);
?>
--EXPECT--
--- For HTML document ---
string(4) "HTML"
string(4) "html"
string(8) "FOO:HTML"
string(8) "foo:html"
string(4) "html"
string(4) "html"
--- For XML document ---
string(4) "html"
string(4) "html"
string(8) "foo:html"
string(8) "foo:html"
string(4) "html"
string(4) "html"
