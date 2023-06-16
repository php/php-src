--TEST--
GH-11404: DOMDocument::savexml and friends ommit xmlns="" declaration for null namespace, creating incorrect xml representation of the DOM
--EXTENSIONS--
dom
--FILE--
<?php

echo "-- Test append and attributes: with default namespace variation --\n";

function testAppendAndAttributes($dom) {
    $nodeA = $dom->createElement('a');
    $nodeB = $dom->createElementNS(null, 'b');
    $nodeC = $dom->createElementNS('', 'c');
    $nodeD = $dom->createElement('d');
    $nodeD->setAttributeNS('some:ns', 'x:attrib', 'val');
    $nodeE = $dom->createElementNS('some:ns', 'e');
    // And these two respect the default ns.
    $nodeE->setAttributeNS(null, 'attrib1', 'val');
    $nodeE->setAttributeNS('', 'attrib2', 'val');

    $dom->documentElement->appendChild($nodeA);
    $dom->documentElement->appendChild($nodeB);
    $dom->documentElement->appendChild($nodeC);
    $dom->documentElement->appendChild($nodeD);
    $dom->documentElement->appendChild($nodeE);

    var_dump($nodeA->namespaceURI);
    var_dump($nodeB->namespaceURI);
    var_dump($nodeC->namespaceURI);
    var_dump($nodeD->namespaceURI);
    var_dump($nodeE->namespaceURI);

    echo $dom->saveXML();

    // Create a subtree without using a fragment
    $subtree = $dom->createElement('subtree');
    $subtree->appendChild($dom->createElementNS('some:ns', 'subtreechild1'));
    $subtree->firstElementChild->appendChild($dom->createElement('subtreechild2'));
    $dom->documentElement->appendChild($subtree);

    echo $dom->saveXML();

    // Create a subtree with the use of a fragment
    $subtree = $dom->createDocumentFragment();
    $subtree->appendChild($child3 = $dom->createElement('child3'));
    $child3->appendChild($dom->createElement('child4'));
    $subtree->appendChild($dom->createElement('child5'));
    $dom->documentElement->appendChild($subtree);

    echo $dom->saveXML();
}

$dom1 = new DOMDocument;
$dom1->loadXML('<?xml version="1.0" ?><with xmlns="some:ns" />');
testAppendAndAttributes($dom1);

echo "-- Test append and attributes: without default namespace variation --\n";

$dom1 = new DOMDocument;
$dom1->loadXML('<?xml version="1.0" ?><with/>');
testAppendAndAttributes($dom1);

echo "-- Test import --\n";

function testImport(?string $href, string $toBeImported) {
    $dom1 = new DOMDocument;
    $decl = $href === NULL ? '' : "xmlns=\"$href\"";
    $dom1->loadXML('<?xml version="1.0" ?><with ' . $decl . '/>');

    $dom2 = new DOMDocument;
    $dom2->loadXML('<?xml version="1.0" ?>' . $toBeImported);

    $dom1->documentElement->append(
        $imported = $dom1->importNode($dom2->documentElement, true)
    );

    var_dump($imported->namespaceURI);

    echo $dom1->saveXML();
}

testImport(null, '<none/>');
testImport('', '<none/>');
testImport('some:ns', '<none/>');
testImport('', '<none><div xmlns="some:ns"/></none>');
testImport('some:ns', '<none xmlns="some:ns"><div xmlns=""/></none>');

echo "-- Namespace URI comparison --\n";

$dom1 = new DOMDocument;
$dom1->loadXML('<?xml version="1.0"?><test xmlns="a:b"><div/></test>');
var_dump($dom1->firstElementChild->namespaceURI);
var_dump($dom1->firstElementChild->firstElementChild->namespaceURI);

$dom1 = new DOMDocument;
$dom1->appendChild($dom1->createElementNS('a:b', 'parent'));
$dom1->firstElementChild->appendChild($dom1->createElementNS('a:b', 'child1'));
$dom1->firstElementChild->appendChild($second = $dom1->createElement('child2'));
var_dump($dom1->firstElementChild->namespaceURI);
var_dump($dom1->firstElementChild->firstElementChild->namespaceURI);
var_dump($second->namespaceURI);
echo $dom1->saveXML();

$dom1 = new DOMDocument;
$dom1->loadXML('<?xml version="1.0"?><test xmlns="a:b"/>');
var_dump($dom1->firstElementChild->namespaceURI);
$dom1->firstElementChild->appendChild($dom1->createElementNS('a:b', 'tag'));
var_dump($dom1->firstElementChild->firstElementChild->namespaceURI);
?>
--EXPECT--
-- Test append and attributes: with default namespace variation --
NULL
NULL
NULL
NULL
string(7) "some:ns"
<?xml version="1.0"?>
<with xmlns="some:ns"><a xmlns=""/><b xmlns=""/><c xmlns=""/><d xmlns:x="some:ns" xmlns="" x:attrib="val"/><e attrib1="val" attrib2="val"/></with>
<?xml version="1.0"?>
<with xmlns="some:ns"><a xmlns=""/><b xmlns=""/><c xmlns=""/><d xmlns:x="some:ns" xmlns="" x:attrib="val"/><e attrib1="val" attrib2="val"/><subtree xmlns=""><subtreechild1 xmlns="some:ns"><subtreechild2 xmlns=""/></subtreechild1></subtree></with>
<?xml version="1.0"?>
<with xmlns="some:ns"><a xmlns=""/><b xmlns=""/><c xmlns=""/><d xmlns:x="some:ns" xmlns="" x:attrib="val"/><e attrib1="val" attrib2="val"/><subtree xmlns=""><subtreechild1 xmlns="some:ns"><subtreechild2 xmlns=""/></subtreechild1></subtree><child3 xmlns=""><child4/></child3><child5 xmlns=""/></with>
-- Test append and attributes: without default namespace variation --
NULL
NULL
NULL
NULL
string(7) "some:ns"
<?xml version="1.0"?>
<with><a/><b/><c/><d xmlns:x="some:ns" x:attrib="val"/><e xmlns="some:ns" attrib1="val" attrib2="val"/></with>
<?xml version="1.0"?>
<with><a/><b/><c/><d xmlns:x="some:ns" x:attrib="val"/><e xmlns="some:ns" attrib1="val" attrib2="val"/><subtree><subtreechild1 xmlns="some:ns"><subtreechild2 xmlns=""/></subtreechild1></subtree></with>
<?xml version="1.0"?>
<with><a/><b/><c/><d xmlns:x="some:ns" x:attrib="val"/><e xmlns="some:ns" attrib1="val" attrib2="val"/><subtree><subtreechild1 xmlns="some:ns"><subtreechild2 xmlns=""/></subtreechild1></subtree><child3><child4/></child3><child5/></with>
-- Test import --
NULL
<?xml version="1.0"?>
<with><none/></with>
NULL
<?xml version="1.0"?>
<with xmlns=""><none/></with>
NULL
<?xml version="1.0"?>
<with xmlns="some:ns"><none xmlns=""/></with>
NULL
<?xml version="1.0"?>
<with xmlns=""><none><div xmlns="some:ns"/></none></with>
string(7) "some:ns"
<?xml version="1.0"?>
<with xmlns="some:ns"><none><div xmlns=""/></none></with>
-- Namespace URI comparison --
string(3) "a:b"
string(3) "a:b"
string(3) "a:b"
string(3) "a:b"
NULL
<?xml version="1.0"?>
<parent xmlns="a:b"><child1/><child2 xmlns=""/></parent>
string(3) "a:b"
string(3) "a:b"
