--TEST--
Test writing Element::$innerHTML on XML documents
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();
$el = $dom->createElementNS('urn:a', 'root');
$dom->appendChild($el);
$el->innerHTML = '<p>foo</p><p xmlns="">bar</p>';
echo $dom->saveXML(), "\n";
$el->innerHTML = '';
echo $dom->saveXML(), "\n";
$el->innerHTML = '&amp;';
echo $dom->saveXML(), "\n";
$el->innerHTML = '&lt;foo&gt;';
echo $dom->saveXML(), "\n";

echo "----------------\n";

$dom = DOM\XMLDocument::createFromString('<root/>');
$child = $dom->documentElement->appendChild($dom->createElementNS('urn:a', 'child'));
$child->setAttributeNS('http://www.w3.org/2000/xmlns/', 'xmlns', 'urn:b');
$child->innerHTML = '<default/>';
echo $dom->saveXML(), "\n";
var_dump($child->namespaceURI);
var_dump($child->firstChild->namespaceURI);

echo "----------------\n";

$dom = DOM\XMLDocument::createFromString(<<<XML
<root xmlns="urn:a" xmlns:b="urn:b" xmlns:c="urn:c">
    <b:child a="none" b:b="b" c:c="c"/>
    <c:child a="none" b:b="b" c:c="c"/>
    <?pi ?>
    <!-- comment -->
    <child a="none" b:b="b" c:c="c">
        <![CDATA[ cdata ]]>
    </child>
</root>
XML);
$dom->documentElement->innerHTML = $dom->documentElement->innerHTML;
echo $dom->saveXML(), "\n";

echo "----------------\n";
$dom->documentElement->innerHTML = <<<XML
<child b:b="b" c:c="c">
    <b:child/>
    <c:child/>
</child>
XML;
echo $dom->saveXML(), "\n";

?>
--EXPECT--
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a"><p>foo</p><p xmlns="">bar</p></root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a"/>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a">&amp;</root>
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a">&lt;foo&gt;</root>
----------------
<?xml version="1.0" encoding="UTF-8"?>
<root><child xmlns="urn:a"><default/></child></root>
string(5) "urn:a"
string(5) "urn:a"
----------------
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a" xmlns:b="urn:b" xmlns:c="urn:c">
    <b:child a="none" b:b="b" c:c="c"/>
    <c:child a="none" b:b="b" c:c="c"/>
    <?pi ?>
    <!-- comment -->
    <child xmlns="urn:a" a="none" b:b="b" c:c="c">
        <![CDATA[ cdata ]]>
    </child>
</root>
----------------
<?xml version="1.0" encoding="UTF-8"?>
<root xmlns="urn:a" xmlns:b="urn:b" xmlns:c="urn:c"><child b:b="b" c:c="c">
    <b:child/>
    <c:child/>
</child></root>
