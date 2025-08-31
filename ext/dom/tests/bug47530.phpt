--TEST--
Bug #47530 (Importing objects into document fragments creates bogus "default" namespace)
--EXTENSIONS--
dom
--FILE--
<?php

function test_document_fragment_with_import() {
    $doc = new DOMDocument;
    $doc->loadXML('<html xmlns="https://php.net/something" xmlns:ns="https://php.net/whatever"><element ns:foo="https://php.net/bar"/></html>');
    $root = $doc->documentElement;
    $frag = $doc->createDocumentFragment();
    $frag->appendChild($doc->importNode($root->firstChild));
    $root->appendChild($frag);
    echo $doc->saveXML();
}

function test_document_fragment_without_import() {
    $doc = new DOMDocument;
    $doc->loadXML('<html xmlns=""><element xmlns:foo="https://php.net/bar"/></html>');
    $frag = $doc->createDocumentFragment();
    $frag->appendChild($doc->createElementNS('https://php.net/bar', 'bar'));
    $frag->appendChild($doc->createElementNS('', 'bar'));
    $element = $doc->documentElement->firstChild;
    $element->appendChild($frag);
    unset($frag); // Free fragment, should not break getting the namespaceURI below
    echo $doc->saveXML();
    unset($doc);
    var_dump($element->firstChild->tagName);
    var_dump($element->firstChild->namespaceURI);
}

function test_document_import() {
    $xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
<div xmlns="http://www.w3.org/1999/xhtml">
    <p>Test-Text</p>
</div>
</feed>
XML;

    $dom = new DOMDocument();
    $dom->loadXML($xml);

    $dom2 = new DOMDocument();
    $importedNode = $dom2->importNode($dom->documentElement, true);
    $dom2->appendChild($importedNode);

    echo $dom2->saveXML();
}

function test_partial_document_import() {
    $xml = <<<XML
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/1999/xhtml" xmlns:test="https://php.net/test" xmlns:example="https://php.net/example">
<div>
    <p>Test-Text</p>
    <example:p>More test text</example:p>
    <test:p>Even more test text</test:p>
</div>
</feed>
XML;

    $dom = new DOMDocument();
    $dom->loadXML($xml);

    $dom2 = new DOMDocument();
    $dom2->loadXML('<?xml version="1.0"?><container xmlns:test="https://php.net/test" xmlns="https://php.net/example"/>');
    $importedNode = $dom2->importNode($dom->documentElement, true);
    $dom2->documentElement->appendChild($importedNode);

    // Freeing the original document shouldn't break the other document
    unset($importedNode);
    unset($dom);

    echo $dom2->saveXML();
}

function test_document_import_with_attributes() {
    $dom = new DOMDocument();
    $dom->loadXML('<?xml version="1.0"?><div xmlns="https://php.net/default" xmlns:example="https://php.net/example"><p example:test="test"/><i/></div>');
    $dom2 = new DOMDocument();
    $dom2->loadXML('<?xml version="1.0"?><div xmlns:example="https://php.net/somethingelse"/>');
    $dom2->documentElement->appendChild($dom2->importNode($dom->documentElement->firstChild));
    echo $dom2->saveXML(), "\n";

    $dom2->documentElement->firstChild->appendChild($dom2->importNode($dom->documentElement->firstChild->nextSibling));
    echo $dom2->saveXML(), "\n";
}

function test_appendChild_with_shadowing() {
    $dom = new DOMDocument();
    $dom->loadXML('<?xml version="1.0"?><container xmlns:default="http://php.net/default"><a xmlns:foo="http://php.net/bar"/><b xmlns:foo="http://php.net/foo"><default:test foo:bar=""/><foo:test2/></b></container>');

    $a = $dom->documentElement->firstElementChild;
    $b = $a->nextSibling;
    $b->remove();
    $a->appendChild($b);

    echo $dom->saveXML(), "\n";
}

echo "-- Test document fragment with import --\n";
test_document_fragment_with_import();
echo "-- Test document fragment without import --\n";
test_document_fragment_without_import();
echo "-- Test document import --\n";
test_document_import();
echo "-- Test partial document import --\n";
test_partial_document_import();
echo "-- Test document import with attributes --\n";
test_document_import_with_attributes();
echo "-- Test appendChild with shadowing --\n";
test_appendChild_with_shadowing();

?>
--EXPECT--
-- Test document fragment with import --
<?xml version="1.0"?>
<html xmlns="https://php.net/something" xmlns:ns="https://php.net/whatever"><default:element xmlns:default="https://php.net/something" ns:foo="https://php.net/bar"/></html>
-- Test document fragment without import --
<?xml version="1.0"?>
<html xmlns=""><element xmlns:foo="https://php.net/bar"><foo:bar/><bar/></element></html>
string(7) "foo:bar"
string(19) "https://php.net/bar"
-- Test document import --
<?xml version="1.0"?>
<feed xmlns="http://www.w3.org/2005/Atom" xmlns:default="http://www.w3.org/1999/xhtml">
<default:div xmlns="http://www.w3.org/1999/xhtml">
    <default:p>Test-Text</default:p>
</default:div>
</feed>
-- Test partial document import --
<?xml version="1.0"?>
<container xmlns:test="https://php.net/test" xmlns="https://php.net/example"><feed xmlns="http://www.w3.org/1999/xhtml" xmlns:example="https://php.net/example">
<div>
    <p>Test-Text</p>
    <example:p>More test text</example:p>
    <test:p>Even more test text</test:p>
</div>
</feed></container>
-- Test document import with attributes --
<?xml version="1.0"?>
<div xmlns:example="https://php.net/somethingelse"><p xmlns="https://php.net/default" xmlns:example="https://php.net/example" example:test="test"/></div>

<?xml version="1.0"?>
<div xmlns:example="https://php.net/somethingelse"><p xmlns="https://php.net/default" xmlns:example="https://php.net/example" example:test="test"><i/></p></div>

-- Test appendChild with shadowing --
<?xml version="1.0"?>
<container xmlns:default="http://php.net/default"><a xmlns:foo="http://php.net/bar"><b xmlns:foo="http://php.net/foo"><default:test foo:bar=""/><foo:test2/></b></a></container>
