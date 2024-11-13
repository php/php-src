--TEST--
Cloning an attribute should retain its namespace 01
--EXTENSIONS--
dom
--FILE--
<?php

function createTestDocument() {
    $dom = new DOMDocument;
    $dom->loadXML('<?xml version="1.0"?><container/>');
    $dom->documentElement->setAttributeNs("some:ns", "foo:bar", "value");

    $attr = $dom->documentElement->getAttributeNodeNs("some:ns", "bar");
    $clone = $attr->cloneNode(true);

    return [$dom, $clone];
}

[$dom, $clone] = createTestDocument();
var_dump($clone->prefix, $clone->namespaceURI);

echo "--- Re-adding a namespaced attribute ---\n";

[$dom, $clone] = createTestDocument();
$dom->documentElement->removeAttributeNs("some:ns", "bar");
echo $dom->saveXML();
$dom->documentElement->setAttributeNodeNs($clone);
echo $dom->saveXML();

echo "--- Re-adding a namespaced attribute, with the namespace deleted (setAttributeNodeNs variation) ---\n";

function readd_test(string $method) {
    [$dom, $clone] = createTestDocument();
    $dom->documentElement->removeAttributeNs("some:ns", "bar");
    $dom->documentElement->removeAttribute("xmlns:foo");
    echo $dom->saveXML();
    $child = $dom->documentElement->appendChild($dom->createElement("child"));
    $child->{$method}($clone);
    echo $dom->saveXML();
}

readd_test("setAttributeNodeNs");

echo "--- Re-adding a namespaced attribute, with the namespace deleted (setAttributeNode variation) ---\n";

readd_test("setAttributeNode");

echo "--- Re-adding a namespaced attribute, with the namespace deleted (appendChild variation) ---\n";

readd_test("appendChild");

echo "--- Removing the document reference should not crash ---\n";

[$dom, $clone] = createTestDocument();
unset($dom);
var_dump($clone->prefix, $clone->namespaceURI);

?>
--EXPECT--
string(3) "foo"
string(7) "some:ns"
--- Re-adding a namespaced attribute ---
<?xml version="1.0"?>
<container xmlns:foo="some:ns"/>
<?xml version="1.0"?>
<container xmlns:foo="some:ns" foo:bar="value"/>
--- Re-adding a namespaced attribute, with the namespace deleted (setAttributeNodeNs variation) ---
<?xml version="1.0"?>
<container/>
<?xml version="1.0"?>
<container><child xmlns:foo="some:ns" foo:bar="value"/></container>
--- Re-adding a namespaced attribute, with the namespace deleted (setAttributeNode variation) ---
<?xml version="1.0"?>
<container/>
<?xml version="1.0"?>
<container><child xmlns:foo="some:ns" foo:bar="value"/></container>
--- Re-adding a namespaced attribute, with the namespace deleted (appendChild variation) ---
<?xml version="1.0"?>
<container/>
<?xml version="1.0"?>
<container><child xmlns:foo="some:ns" foo:bar="value"/></container>
--- Removing the document reference should not crash ---
string(3) "foo"
string(7) "some:ns"
