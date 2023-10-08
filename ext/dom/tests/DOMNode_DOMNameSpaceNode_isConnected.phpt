--TEST--
DOMNode::isConnected and DOMNameSpaceNode::isConnected
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument();
$dom->loadXML('<!DOCTYPE html><html><head/><body/></html>');

$docElement = $dom->documentElement;
$head = $docElement->firstChild;
$body = $head->nextSibling;

echo "--- Created element not connected yet ---\n";

$p = $dom->createElement('p');
var_dump($p->isConnected);

echo "--- Appending and checking connection isn't broken for parents ---\n";

$body->appendChild($p);
var_dump($body->isConnected);
var_dump($p->isConnected);
$document = $docElement->parentNode;
var_dump($document->isConnected);
var_dump($dom->doctype->isConnected);

echo "--- Indirect removal should set isConnected to false for affected nodes ---\n";

$body->remove();
var_dump($p->isConnected);
var_dump($docElement->isConnected);
var_dump($body->isConnected);
var_dump($head->isConnected);
var_dump($dom->doctype->isConnected);

echo "--- Empty document test ---\n";

$dom = new DOMDocument();
var_dump($dom->isConnected);

?>
--EXPECT--
--- Created element not connected yet ---
bool(false)
--- Appending and checking connection isn't broken for parents ---
bool(true)
bool(true)
bool(true)
bool(true)
--- Indirect removal should set isConnected to false for affected nodes ---
bool(false)
bool(true)
bool(false)
bool(true)
bool(true)
--- Empty document test ---
bool(true)
