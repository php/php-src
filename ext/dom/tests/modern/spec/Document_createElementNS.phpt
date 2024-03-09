--TEST--
Document::createElementNS()
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/element_dump.inc";
require __DIR__ . "/create_element_util.inc";

echo "--- Normal cases ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$elements = [];

$elements[] = createElementNS($dom, NULL, "qname", "&hello");
$elements[] = createElementNS($dom, "urn:foo", "foo:bar", "&hello");
$elements[] = createElementNS($dom, "urn:foo", "bar:BAR", "&hello");
$elements[] = createElementNS($dom, "urn:foo", "bar:BAR", "&hello");
$elements[] = createElementNS($dom, "http://www.w3.org/2000/xmlns/", "xmlns", "&hello");
$elements[] = createElementNS($dom, "http://www.w3.org/XML/1998/namespace", "xml", "&hello");

foreach ($elements as $element) {
    dumpElement($element);
}

echo "--- Error cases ---\n";

function testError($dom, $namespaceURI, $qualifiedName)
{
    $ns_readable = is_null($namespaceURI) ? "NULL" : "\"$namespaceURI\"";
    echo "($ns_readable, \"$qualifiedName\"): ";
    try {
        $dom->createElementNS($namespaceURI, $qualifiedName);
    } catch (DOMException $e) {
        echo $e->getMessage(), "\n";
    }
}

testError($dom, NULL, "prefix:name");
testError($dom, "", "prefix:name");
testError($dom, "urn:foo", "@");
testError($dom, "http://www.w3.org/2000/xmlns/", "svg");
testError($dom, "urn:foo", "xml:xml");

?>
--EXPECT--
--- Normal cases ---
tagName: string(5) "qname"
nodeName: string(5) "qname"
textContent: string(6) "&hello"
prefix: NULL
namespaceURI: NULL
<qname>&amp;hello</qname>

tagName: string(7) "foo:bar"
nodeName: string(7) "foo:bar"
textContent: string(6) "&hello"
prefix: string(3) "foo"
namespaceURI: string(7) "urn:foo"
<foo:bar>&amp;hello</foo:bar>

tagName: string(7) "bar:BAR"
nodeName: string(7) "bar:BAR"
textContent: string(6) "&hello"
prefix: string(3) "bar"
namespaceURI: string(7) "urn:foo"
<bar:BAR>&amp;hello</bar:BAR>

tagName: string(7) "bar:BAR"
nodeName: string(7) "bar:BAR"
textContent: string(6) "&hello"
prefix: string(3) "bar"
namespaceURI: string(7) "urn:foo"
<bar:BAR>&amp;hello</bar:BAR>

tagName: string(5) "xmlns"
nodeName: string(5) "xmlns"
textContent: string(6) "&hello"
prefix: NULL
namespaceURI: string(29) "http://www.w3.org/2000/xmlns/"
<xmlns>&amp;hello</xmlns>

tagName: string(3) "xml"
nodeName: string(3) "xml"
textContent: string(6) "&hello"
prefix: NULL
namespaceURI: string(36) "http://www.w3.org/XML/1998/namespace"
<xml>&amp;hello</xml>

--- Error cases ---
(NULL, "prefix:name"): Namespace Error
("", "prefix:name"): Namespace Error
("urn:foo", "@"): Invalid Character Error
("http://www.w3.org/2000/xmlns/", "svg"): Namespace Error
("urn:foo", "xml:xml"): Namespace Error
