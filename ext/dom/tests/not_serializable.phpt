--TEST--
DOM classes are not serializable
--EXTENSIONS--
dom
--FILE--
<?php

$doc = new DOMDocument();
$doc->loadXML('<root><node/></root>');
try {
    serialize($doc);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

$node = $doc->documentElement;
try {
    serialize($node);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

$xpath = new DOMXPath($doc);
try {
    serialize($xpath);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

$ns = $xpath->query('//namespace::*')->item(0);
try {
    serialize($ns);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

$dom = DOM\XMLDocument::createEmpty();
try {
    serialize($dom);
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

try {
    serialize(new DOM\XPath($dom));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Serialization of 'DOMDocument' is not allowed, unless serialization methods are implemented in a subclass
Serialization of 'DOMElement' is not allowed, unless serialization methods are implemented in a subclass
Serialization of 'DOMXPath' is not allowed
Serialization of 'DOMNameSpaceNode' is not allowed, unless serialization methods are implemented in a subclass
Serialization of 'DOM\XMLDocument' is not allowed, unless serialization methods are implemented in a subclass
Serialization of 'DOM\XPath' is not allowed
