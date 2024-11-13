--TEST--
HTMLDocument: XML-style namespace
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createFromString(<<<HTML
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
$xpath = new Dom\XPath($dom);
foreach ($xpath->query("//*[name()='body']//*") as $node) {
    echo $node->nodeName, " ", $node->namespaceURI ?? "(NONE)", "\n";
    echo "prefix: \"", $node->prefix, "\"\n";
    foreach ($node->attributes as $attribute) {
        echo "  Attribute: ", $attribute->nodeName, " ", $attribute->namespaceURI ?? "(NONE)", "\n";
    }
}

echo "--- HTML serialization ---\n";
echo $dom->saveHtml(), "\n";
echo "--- XML serialization ---\n";
echo $dom->saveXml();

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
