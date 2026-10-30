--TEST--
DOMNode::parentElement and DOMNameSpaceNode::parentElement
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML(<<<XML
<!DOCTYPE HTML>
<html>
    <body/>
</html>
XML);

echo "--- body test ---\n";

$body = $dom->documentElement->firstElementChild;
var_dump($body->parentNode->localName);
var_dump($body->parentElement->localName);

echo "--- document test ---\n";

var_dump(get_class($dom->documentElement->parentNode));
var_dump($dom->documentElement->parentElement);

var_dump(get_class($dom->doctype->parentNode));
var_dump($dom->doctype->parentElement);

echo "--- fragment test ---\n";

$fragment = $dom->createDocumentFragment();
$p = $fragment->appendChild($dom->createElement('p'));

var_dump(get_class($p->parentNode));
var_dump($p->parentElement);

$body->appendChild($fragment);

var_dump($p->parentNode->localName);
var_dump($p->parentElement->localName);

?>
--EXPECT--
--- body test ---
string(4) "html"
string(4) "html"
--- document test ---
string(11) "DOMDocument"
NULL
string(11) "DOMDocument"
NULL
--- fragment test ---
string(19) "DOMDocumentFragment"
NULL
string(4) "body"
string(4) "body"
