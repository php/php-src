--TEST--
Bug #80927 (Removing documentElement after creating attribute node: possible use-after-free)
--EXTENSIONS--
dom
--FILE--
<?php

function test1() {
    $dom = new DOMDocument();
    $dom->appendChild($dom->createElement("html"));
    $a = $dom->createAttributeNS("fake_ns", "test:test");
    $dom->removeChild($dom->documentElement);

    echo $dom->saveXML();

    var_dump($a->namespaceURI);
    var_dump($a->prefix);
}

enum Test2Variation {
    case REMOVE_DOCUMENT;
    case REMOVE_CHILD;
}

function test2(Test2Variation $variation) {
    $dom = new DOMDocument();
    $dom->appendChild($dom->createElement("html"));
    $a = $dom->createAttributeNS("fake_ns", "test:test");

    $foo = $dom->appendChild($dom->createElement('foo'));
    $foo->appendChild($dom->documentElement);

    unset($foo);

    match ($variation) {
        Test2Variation::REMOVE_DOCUMENT => $dom->documentElement->remove(),
        Test2Variation::REMOVE_CHILD => $dom->documentElement->firstElementChild->remove(),
    };

    echo $dom->saveXML();

    var_dump($a->namespaceURI);
    var_dump($a->prefix);
}

function test3() {
    $dom = new DOMDocument();
    $dom->appendChild($dom->createElement('html'));
    $foobar = $dom->documentElement->appendChild($dom->createElementNS('some:ns', 'foo:bar'));
    $foobar2 = $foobar->appendChild($dom->createElementNS('some:ns', 'foo:bar2'));
    $foobar->remove();
    unset($foobar);
    $dom->documentElement->appendChild($foobar2);

    echo $dom->saveXML();

    var_dump($foobar2->namespaceURI);
    var_dump($foobar2->prefix);
}

echo "--- Remove namespace declarator for attribute, root ---\n";
test1();
echo "--- Remove namespace declarator for attribute, moved root ---\n";
test2(Test2Variation::REMOVE_DOCUMENT);
echo "--- Remove namespace declarator for attribute, moved root child ---\n";
test2(Test2Variation::REMOVE_CHILD);
echo "--- Remove namespace declarator for element ---\n";
test3();

?>
--EXPECT--
--- Remove namespace declarator for attribute, root ---
<?xml version="1.0"?>
string(7) "fake_ns"
string(4) "test"
--- Remove namespace declarator for attribute, moved root ---
<?xml version="1.0"?>
string(7) "fake_ns"
string(4) "test"
--- Remove namespace declarator for attribute, moved root child ---
<?xml version="1.0"?>
<foo/>
string(7) "fake_ns"
string(4) "test"
--- Remove namespace declarator for element ---
<?xml version="1.0"?>
<html><foo:bar2 xmlns:foo="some:ns"/></html>
string(7) "some:ns"
string(3) "foo"
