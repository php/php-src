--TEST--
HTMLDocument: XML-style namespace
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
<head>
    <title>Test</title>
</head>
<body xmlns:foo="urn:hi">
    <foo:bar></foo:bar>
</body>
</html>
HTML);

echo "--- Namespaces ---\n";
$xpath = new DOM\XPath($dom);
foreach ($xpath->query("//*[name()='body']//*") as $node) {
    echo $node->nodeName, " ", $node->namespaceURI ?? "(NONE)", "\n";
    echo "prefix: \"", $node->prefix, "\"\n";
    foreach ($node->attributes as $attribute) {
        echo "  Attribute: ", $attribute->nodeName, " ", $attribute->namespaceURI ?? "(NONE)", "\n";
    }
}

echo "--- HTML serialization ---\n";
echo $dom->saveHTML(), "\n";
echo "--- XML serialization ---\n";
echo $dom->saveXML();

?>
--EXPECT--
--- Namespaces ---
FOO:BAR http://www.w3.org/1999/xhtml
prefix: ""
--- HTML serialization ---
<!DOCTYPE html><html><head>
    <title>Test</title>
</head>
<body xmlns:foo="urn:hi">
    <foo:bar></foo:bar>

</body></html>
--- XML serialization ---
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml"><head>
    <title>Test</title>
</head>
<body xmlns:foo="urn:hi">
    <foo:bar></foo:bar>

</body></html>
