--TEST--
XML serializer spec errata: xmlns="" serialization should be allowed
--EXTENSIONS--
dom
--FILE--
<?php

// Should be allowed
$dom = Dom\XMLDocument::createFromString('<root><x xmlns=""/></root>');
var_dump($dom->documentElement->innerHTML);

// Should not be allowed
$dom = Dom\XMLDocument::createFromString('<root><x/></root>');
$x = $dom->documentElement->firstChild;
$x->setAttributeNS('http://www.w3.org/2000/xmlns/', 'xmlns:a', '');
try {
    var_dump($dom->documentElement->innerHTML);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
string(13) "<x xmlns=""/>"
The resulting XML serialization is not well-formed
