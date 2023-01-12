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

?>
--EXPECT--
Serialization of 'DOMDocument' is not allowed, unless you extend the class and provide a serialisation method
Serialization of 'DOMElement' is not allowed, unless you extend the class and provide a serialisation method
Serialization of 'DOMXPath' is not allowed, unless you extend the class and provide a serialisation method
Serialization of 'DOMNameSpaceNode' is not allowed, unless you extend the class and provide a serialisation method
