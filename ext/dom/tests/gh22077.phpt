--TEST--
GH-22077 (UAF in custom XPath function)
--FILE--
<?php
$document = new DOMDocument;
$xpath = new DOMXPath($document);
$xpath->registerNamespace("my", "my.ns");
$xpath->registerPhpFunctionNS('my.ns', 'include', function(): DOMElement {
    $includedDocument = new DOMDocument;
    $includedDocument->loadXML('<root><uaf/><node/><uaf/></root>');
    return $includedDocument->documentElement;
});
$nodeset = $xpath->query('my:include()/uaf');
$node = $nodeset->item(0);
var_dump($nodeset->length);
var_dump($node->ownerDocument->saveXML($node));
?>
--EXPECT--
int(2)
string(6) "<uaf/>"
