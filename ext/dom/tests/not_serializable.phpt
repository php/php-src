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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$node = $doc->documentElement;
try {
    serialize($node);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$xpath = new DOMXPath($doc);
try {
    serialize($xpath);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$ns = $xpath->query('//namespace::*')->item(0);
try {
    serialize($ns);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

$dom = Dom\XMLDocument::createEmpty();
try {
    serialize($dom);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    serialize(new Dom\XPath($dom));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Exception: Serialization of 'DOMDocument' is not allowed, unless serialization methods are implemented in a subclass
Exception: Serialization of 'DOMElement' is not allowed, unless serialization methods are implemented in a subclass
Exception: Serialization of 'DOMXPath' is not allowed
Exception: Serialization of 'DOMNameSpaceNode' is not allowed, unless serialization methods are implemented in a subclass
Exception: Serialization of 'Dom\XMLDocument' is not allowed, unless serialization methods are implemented in a subclass
Exception: Serialization of 'Dom\XPath' is not allowed
