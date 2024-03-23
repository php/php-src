--TEST--
Test DOM\Document::$head
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- From parsing ---\n";

$dom = DOM\HTMLDocument::createFromString("<p>foo</p>", LIBXML_NOERROR);
var_dump($dom->head?->nodeName);

echo "--- After head removal ---\n";

$dom->head->remove();
var_dump($dom->head?->nodeName);

echo "--- head in no namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("", "head"));
var_dump($dom->head?->nodeName);
$tmp->remove();

echo "--- head in right namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "head"));
var_dump($dom->head?->nodeName);
$tmp->remove();

echo "--- prefixed head in right namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix:head"));
var_dump($dom->head?->nodeName);
$tmp->remove();

echo "--- multiple head elements in right namespace ---\n";

$tmp1 = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix1:head"));
var_dump($dom->head?->nodeName);
$tmp2 = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix2:head"));
var_dump($dom->head?->nodeName);
$tmp1->remove();
var_dump($dom->head?->nodeName);
$tmp2->remove();
var_dump($dom->head?->nodeName);

echo "--- html element in no namespace ---\n";

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="">
  <head/>
</html>
XML);
var_dump($dom->head);

?>
--EXPECT--
--- From parsing ---
string(4) "HEAD"
--- After head removal ---
NULL
--- head in no namespace ---
NULL
--- head in right namespace ---
string(4) "HEAD"
--- prefixed head in right namespace ---
string(11) "PREFIX:HEAD"
--- multiple head elements in right namespace ---
string(12) "PREFIX1:HEAD"
string(12) "PREFIX1:HEAD"
string(12) "PREFIX2:HEAD"
NULL
--- html element in no namespace ---
NULL
