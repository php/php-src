--TEST--
DOMDocument::createAttributeNS()
--EXTENSIONS--
dom
--XFAIL--
Won't work until setAttributeNode(NS) is fixed
--FILE--
<?php

function dumpAttr(DOM\Attr $attr) {
    echo "Attr: {$attr->name}\n";
    var_dump($attr->prefix);
    var_dump($attr->nodeName);
    var_dump($attr->namespaceURI);
}

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
