--TEST--
Document::importLegacyNode
--EXTENSIONS--
dom
--FILE--
<?php

$old = new DOMDocument;
$old->loadXML(<<<XML
<root>
    <child xmlns="urn:a" a="b"/>
    <child xmlns="urn:b" xmlns:c="urn:c" c:c="d"/>
    <?pi?>
    <!-- comment -->
    <![CDATA[foo]]>
</root>
XML);

$new = DOM\XMLDocument::createEmpty();
$new->append($new->importLegacyNode($old->documentElement, true));

unset($old);

foreach ($new->getElementsByTagName('child') as $child) {
    var_dump($child->attributes);
    foreach ($child->attributes as $attr) {
        echo "name: ";
        var_dump($attr->name);
        echo "prefix: ";
        var_dump($attr->prefix);
        echo "namespaceURI: ";
        var_dump($attr->namespaceURI);
    }
}

echo $new->saveXML(), "\n";

?>
--EXPECT--
object(DOM\NamedNodeMap)#5 (1) {
  ["length"]=>
  int(2)
}
name: string(5) "xmlns"
prefix: string(0) ""
namespaceURI: string(29) "http://www.w3.org/2000/xmlns/"
name: string(1) "a"
prefix: string(0) ""
namespaceURI: NULL
object(DOM\NamedNodeMap)#3 (1) {
  ["length"]=>
  int(3)
}
name: string(5) "xmlns"
prefix: string(0) ""
namespaceURI: string(29) "http://www.w3.org/2000/xmlns/"
name: string(7) "xmlns:c"
prefix: string(5) "xmlns"
namespaceURI: string(29) "http://www.w3.org/2000/xmlns/"
name: string(3) "c:c"
prefix: string(1) "c"
namespaceURI: string(5) "urn:c"
<?xml version="1.0" encoding="UTF-8"?>
<root>
    <child xmlns="urn:a" a="b"/>
    <child xmlns="urn:b" xmlns:c="urn:c" c:c="d"/>
    <?pi ?>
    <!-- comment -->
    <![CDATA[foo]]>
</root>
