--TEST--
DOM\XMLDocument interaction with XPath
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString(<<<XML
<?xml version="1.0"?>
<root>
    <p>hi</p>
    <element xmlns:a="urn:a">
        <?target data?>
        <!-- comment -->
    </element>
</root>
XML);

$xpath = new DOM\XPath($dom);

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

// TODO: broken
$ns = $xpath->evaluate("/*/namespace::*");
var_dump($ns);
var_dump($ns->item(0));

?>
--EXPECT--
--- Get data of the paragraph ---
object(DOM\NodeList)#4 (1) {
  ["length"]=>
  int(1)
}
string(2) "hi"
object(DOM\NodeList)#5 (1) {
  ["length"]=>
  int(1)
}
string(2) "hi"
string(2) "hi"
float(2)
bool(true)
--- Get data of special nodes ---
string(25) "DOM\ProcessingInstruction"
string(4) "data"
string(11) "DOM\Comment"
string(9) " comment "
--- Get a namespace node ---
