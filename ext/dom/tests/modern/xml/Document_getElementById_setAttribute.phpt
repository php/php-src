--TEST--
Dom\XMLDocument::getElementById() after setting id with setAttribute()
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createEmpty();
$root = $dom->appendChild($dom->createElement("root"));
$a = $dom->createElement("a");
$dom->documentElement->appendChild($a);

echo "--- After parsing ---\n";
var_dump($dom->getElementById("x")?->nodeName);

echo "--- After setAttribute ---\n";
$a->setAttribute("id", "x");
var_dump($dom->getElementById("x")?->nodeName);
var_dump($dom->getElementById("y")?->nodeName);

echo "--- After changing the id value with setAttribute ---\n";
$a->setAttribute("id", "y");
var_dump($dom->getElementById("x")?->nodeName);
var_dump($dom->getElementById("y")?->nodeName);

echo "--- After setAttributeNS ---\n";
$a->setAttributeNS(null, "id", "z");
var_dump($dom->getElementById("y")?->nodeName);
var_dump($dom->getElementById("z")?->nodeName);

echo "--- After removing the attribute ---\n";
$a->removeAttribute("id");
var_dump($dom->getElementById("z")?->nodeName);

echo "--- After setIdAttribute ---\n";
$a->setAttribute("id", "w");
$a->setIdAttribute("id", false);
var_dump($dom->getElementById("w")?->nodeName);

?>
--EXPECT--
--- After parsing ---
NULL
--- After setAttribute ---
string(1) "a"
NULL
--- After changing the id value with setAttribute ---
NULL
string(1) "a"
--- After setAttributeNS ---
NULL
string(1) "a"
--- After removing the attribute ---
NULL
--- After setIdAttribute ---
NULL
