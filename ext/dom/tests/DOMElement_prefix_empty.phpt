--TEST--
DOMElement->prefix with empty string creates bogus prefix
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOMDocument;
$dom->loadXML("<hello:container xmlns:conflict=\"urn:foo\" xmlns:hello=\"http://www.w3.org/1999/xhtml\"/>");

$container = $dom->documentElement;

echo "--- Changing the prefix to an empty string ---\n";

$container->prefix = "";
echo $dom->saveXML();

echo "--- Changing the prefix to an empty C-style string ---\n";

$container->prefix = "\0foobar";
echo $dom->saveXML();

echo "--- Changing the prefix to \"hello\" ---\n";

$container->prefix = "hello";
echo $dom->saveXML();

echo "--- Changing the prefix to that of a conflicting namespace (\"conflict\") ---\n";

try {
    $container->prefix = "conflict";
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
echo $dom->saveXML();

?>
--EXPECT--
--- Changing the prefix to an empty string ---
<?xml version="1.0"?>
<container xmlns:conflict="urn:foo" xmlns:hello="http://www.w3.org/1999/xhtml" xmlns="http://www.w3.org/1999/xhtml"/>
--- Changing the prefix to an empty C-style string ---
<?xml version="1.0"?>
<container xmlns:conflict="urn:foo" xmlns:hello="http://www.w3.org/1999/xhtml" xmlns="http://www.w3.org/1999/xhtml"/>
--- Changing the prefix to "hello" ---
<?xml version="1.0"?>
<hello:container xmlns:conflict="urn:foo" xmlns:hello="http://www.w3.org/1999/xhtml" xmlns="http://www.w3.org/1999/xhtml"/>
--- Changing the prefix to that of a conflicting namespace ("conflict") ---
Namespace Error
<?xml version="1.0"?>
<hello:container xmlns:conflict="urn:foo" xmlns:hello="http://www.w3.org/1999/xhtml" xmlns="http://www.w3.org/1999/xhtml"/>
