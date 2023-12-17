--TEST--
Element::setAttributeNS()
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . "/dump_attr.inc";

$dom = DOM\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElement("container"));

echo "--- xmlns attribute ---\n";

$container->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:foo", "1");
echo $dom->saveHTML($container), "\n";
dumpAttrs($container);

echo "--- name validation ---\n";

try {
    $container->setAttributeNS("urn:a", "a:b:c", "");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo "--- ns attributes with same namespace but different prefix ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElement("container"));

$container->setAttributeNS("urn:a", "x:foo", "1");
$container->setAttributeNS("urn:a", "y:foo", "2");
echo $dom->saveHTML($container), "\n";
dumpAttrs($container);

echo "--- ns attributes with different namespace but same prefix ---\n";

$dom = DOM\HTMLDocument::createEmpty();
$container = $dom->appendChild($dom->createElement("container"));

$container->setAttributeNS("urn:a", "x:foo", "1");
$container->setAttributeNS("urn:b", "x:foo", "2");
echo $dom->saveHTML($container), "\n";
dumpAttrs($container);

?>
--EXPECT--
--- xmlns attribute ---
<container xmlns:foo="1"></container>
Attr: foo
string(5) "xmlns"
string(9) "xmlns:foo"
string(29) "http://www.w3.org/2000/xmlns/"
--- name validation ---
Invalid Character Error
--- ns attributes with same namespace but different prefix ---
<container x:foo="1" y:foo="2"></container>
Attr: foo
string(1) "x"
string(5) "x:foo"
string(5) "urn:a"
Attr: foo
string(1) "y"
string(5) "y:foo"
string(5) "urn:a"
--- ns attributes with different namespace but same prefix ---
<container x:foo="1" x:foo="2"></container>
Attr: foo
string(1) "x"
string(5) "x:foo"
string(5) "urn:a"
Attr: foo
string(1) "x"
string(5) "x:foo"
string(5) "urn:b"
