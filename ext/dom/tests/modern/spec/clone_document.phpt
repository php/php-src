--TEST--
Cloning a whole document
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\XMLDocument::createFromString(<<<XML
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
    <!ELEMENT child (#PCDATA)>
    <!ELEMENT child2 (#PCDATA)>
    <!ENTITY foo "bar">
]>
<root xmlns:a="urn:a" a:foo="bar">
    afoob
    <child>
        <![CDATA[c]]>
    </child>
    <child2>&foo;</child2>
</root>
XML);

echo "---\n";

var_dump($dom->getElementsByTagName("child2")[0]->firstChild->nodeName);

echo "---\n";

$clone = clone $dom;
echo $clone->saveXml(), "\n";

var_dump($clone->getElementsByTagName("child2")[0]->firstChild->nodeName);

echo "---\n";

$clone = $dom->cloneNode(false);
echo $clone->saveXml(), "\n";

echo "---\n";

$clone = $dom->documentElement->cloneNode(false);
echo $clone->ownerDocument->saveXml($clone), "\n";

?>
--EXPECT--
---
string(3) "foo"
---
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE root [
<!ELEMENT child (#PCDATA)>
<!ELEMENT child2 (#PCDATA)>
<!ENTITY foo "bar">
]>
<root xmlns:a="urn:a" a:foo="bar">
    afoob
    <child>
        <![CDATA[c]]>
    </child>
    <child2>&foo;</child2>
</root>
string(3) "foo"
---
<?xml version="1.0" encoding="UTF-8"?>

---
<root xmlns:a="urn:a" a:foo="bar"/>
