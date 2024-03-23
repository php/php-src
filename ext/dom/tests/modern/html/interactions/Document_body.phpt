--TEST--
Test DOM\Document::$body
--EXTENSIONS--
dom
--FILE--
<?php

echo "--- From parsing ---\n";

$dom = DOM\HTMLDocument::createFromString("<p>foo</p>", LIBXML_NOERROR);
var_dump($dom->body?->nodeName);

echo "--- After body removal ---\n";

$dom->body->remove();
var_dump($dom->body?->nodeName);

echo "--- body in no namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("", "body"));
var_dump($dom->body?->nodeName);
$tmp->remove();

echo "--- frameset in no namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("", "frameset"));
var_dump($dom->body?->nodeName);
$tmp->remove();

echo "--- body in right namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "body"));
var_dump($dom->body?->nodeName);
$tmp->remove();

echo "--- frameset in right namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "frameset"));
var_dump($dom->body?->nodeName);
$tmp->remove();

echo "--- prefixed body in right namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix:body"));
var_dump($dom->body?->nodeName);
$tmp->remove();

echo "--- prefixed frameset in right namespace ---\n";

$tmp = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix:frameset"));
var_dump($dom->body?->nodeName);
$tmp->remove();

echo "--- multiple body-like elements in right namespace ---\n";

$tmp1 = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix1:body"));
var_dump($dom->body?->nodeName);
$tmp2 = $dom->documentElement->appendChild($dom->createElementNS("http://www.w3.org/1999/xhtml", "prefix2:frameset"));
var_dump($dom->body?->nodeName);
$tmp1->remove();
var_dump($dom->body?->nodeName);
$tmp2->remove();
var_dump($dom->body?->nodeName);

echo "--- html element in no namespace ---\n";

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="">
  <body/>
</html>
XML);
var_dump($dom->body);

?>
--EXPECT--
--- From parsing ---
string(4) "BODY"
--- After body removal ---
NULL
--- body in no namespace ---
NULL
--- frameset in no namespace ---
NULL
--- body in right namespace ---
string(4) "BODY"
--- frameset in right namespace ---
string(8) "FRAMESET"
--- prefixed body in right namespace ---
string(11) "PREFIX:BODY"
--- prefixed frameset in right namespace ---
string(15) "PREFIX:FRAMESET"
--- multiple body-like elements in right namespace ---
string(12) "PREFIX1:BODY"
string(12) "PREFIX1:BODY"
string(16) "PREFIX2:FRAMESET"
NULL
--- html element in no namespace ---
NULL
