--TEST--
Dom\XMLDocument interaction with XPath
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<root>
    <p>hi</p>
    <element xmlns="urn:e" xmlns:a="urn:a">
        <?target data?>
        <!-- comment -->
    </element>
</root>
XML);

$xpath = new Dom\XPath($dom);

echo "--- Get data of the paragraph ---\n";

$result = $xpath->query("//p");
var_dump($result);
var_dump($result->item(0)->textContent);

$result = $xpath->evaluate("//p");
var_dump($result);
var_dump($result->item(0)->textContent);

var_dump($xpath->evaluate("string(//p)"));
var_dump($xpath->evaluate("string-length(//p)"));
var_dump($xpath->evaluate("boolean(//p)"));

echo "--- Get data of special nodes ---\n";

$result = $xpath->query("//*/comment()|//*/processing-instruction()");
foreach ($result as $item) {
    var_dump(get_class($item));
    var_dump($item->textContent);
}

echo "--- Get a namespace node ---\n";

// Namespace nodes don't exist in modern day DOM.
try {
    var_dump($xpath->evaluate("//*/namespace::*"));
} catch (DOMException $e) {
    echo $e->getCode(), ": ", $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Get data of the paragraph ---
object(Dom\NodeList)#4 (1) {
  ["length"]=>
  int(1)
}
string(2) "hi"
object(Dom\NodeList)#5 (1) {
  ["length"]=>
  int(1)
}
string(2) "hi"
string(2) "hi"
float(2)
bool(true)
--- Get data of special nodes ---
string(25) "Dom\ProcessingInstruction"
string(4) "data"
string(11) "Dom\Comment"
string(9) " comment "
--- Get a namespace node ---
9: The namespace axis is not well-defined in the living DOM specification. Use Dom\Element::getInScopeNamespaces() or Dom\Element::getDescendantNamespaces() instead.
