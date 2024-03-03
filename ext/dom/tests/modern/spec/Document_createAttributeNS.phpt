--TEST--
Document::createAttributeNS()
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/dump_attr.inc";

function testErrorCase($dom, $ns, $qname) {
    try {
        $dom->createAttributeNS($ns, $qname);
    } catch (DOMException $e) {
        $ns_readable = is_null($ns) ? 'null' : "\"$ns\"";
        echo "($ns_readable, \"$qname\"): {$e->getMessage()}\n";
    }

}

$dom = DOM\HTMLDocument::createEmpty();
$attrs = [];
$attrs[] = $dom->createAttributeNS('http://www.w3.org/2000/xmlns/', 'xmlns:foo');
$attrs[] = $dom->createAttributeNS('http://www.w3.org/2000/xmlns/', 'xmlns:bar');
$attrs[] = $dom->createAttributeNS('http://www.w3.org/2000/xmlns/', 'xmlns');
$attrs[] = $dom->createAttributeNS('http://www.w3.org/XML/1998/namespace', 'xml:foo');
$attrs[] = $dom->createAttributeNS('urn:a', 'foo:bar');
$attrs[] = $dom->createAttributeNS('urn:a', 'bar:bar');

echo "--- Error cases ---\n";

testErrorCase($dom, '', 'bar:bar');
testErrorCase($dom, null, 'bar:bar');
testErrorCase($dom, 'urn:a', '@');
testErrorCase($dom, 'urn:a', 'foo:bar:baz');
testErrorCase($dom, 'http://www.w3.org/2000/xmlns', 'xmlns');
testErrorCase($dom, 'http://www.w3.org/2000/xmlns', 'xmlns:bar');
testErrorCase($dom, 'http://www.w3.org/2000/xmlns', 'foo:bar');
testErrorCase($dom, 'http://www.w3.org/2000/xmlns', 'xml:foo');

echo "\n--- Normal cases ---\n";

// Test after creating to make sure they cannot corrupt each other
foreach ($attrs as $attr) {
    dumpAttr($attr);
}

// Test in document
$root = $dom->appendChild($dom->createElement('foo'));
foreach ($attrs as $attr) {
    $root->setAttributeNodeNS($attr);
}

echo $dom->saveHTML(), "\n";

echo "\n--- NULL prefix cases ---\n";

// Test multiple "null prefixes" after having a root
$attrs = [];
$attrs[] = $dom->createAttributeNS(null, 'baz1');
$attrs[] = $dom->createAttributeNS(null, 'baz2');
$attrs[] = $dom->createAttributeNS('', 'baz1');
$attrs[] = $dom->createAttributeNS('', 'baz2');
foreach ($attrs as $attr) {
    dumpAttr($attr);
    $root->setAttributeNodeNS($attr);
}

echo $dom->saveHTML(), "\n";
?>
--EXPECT--
--- Error cases ---
("", "bar:bar"): Namespace Error
(null, "bar:bar"): Namespace Error
("urn:a", "@"): Invalid Character Error
("urn:a", "foo:bar:baz"): Invalid Character Error
("http://www.w3.org/2000/xmlns", "xmlns"): Namespace Error
("http://www.w3.org/2000/xmlns", "xmlns:bar"): Namespace Error
("http://www.w3.org/2000/xmlns", "xml:foo"): Namespace Error

--- Normal cases ---
Attr: xmlns:foo
string(5) "xmlns"
string(9) "xmlns:foo"
string(29) "http://www.w3.org/2000/xmlns/"
Attr: xmlns:bar
string(5) "xmlns"
string(9) "xmlns:bar"
string(29) "http://www.w3.org/2000/xmlns/"
Attr: xmlns
NULL
string(5) "xmlns"
string(29) "http://www.w3.org/2000/xmlns/"
Attr: xml:foo
string(3) "xml"
string(7) "xml:foo"
string(36) "http://www.w3.org/XML/1998/namespace"
Attr: foo:bar
string(3) "foo"
string(7) "foo:bar"
string(5) "urn:a"
Attr: bar:bar
string(3) "bar"
string(7) "bar:bar"
string(5) "urn:a"
<foo xmlns:foo="" xmlns:bar="" xmlns="" xml:foo="" bar:bar=""></foo>

--- NULL prefix cases ---
Attr: baz1
NULL
string(4) "baz1"
NULL
Attr: baz2
NULL
string(4) "baz2"
NULL
Attr: baz1
NULL
string(4) "baz1"
NULL
Attr: baz2
NULL
string(4) "baz2"
NULL
<foo xmlns:foo="" xmlns:bar="" xmlns="" xml:foo="" bar:bar="" baz1="" baz2=""></foo>
