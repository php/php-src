--TEST--
GH-12850 (DOMNameSpaceNode parent not unset when namespace gets unlinked)
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML("<container xmlns=\"urn:foo\"/>");
$container = $dom->documentElement;
$ns = $container->getAttributeNode("xmlns");

echo "--- Before removal ---\n";

var_dump(is_null($ns->parentNode));
var_dump($ns->isConnected);

$container->removeAttributeNs("urn:foo", "");

echo "--- After removal ---\n";

var_dump(is_null($ns->parentNode));
var_dump($ns->isConnected);

?>
--EXPECT--
--- Before removal ---
bool(false)
bool(true)
--- After removal ---
bool(true)
bool(false)
