--TEST--
registerNodeClass() with an abstract class should fail
--EXTENSIONS--
dom
--FILE--
<?php

abstract class Test extends DOMElement {
    abstract function foo();
}

$dom = new DOMDocument;

try {
    $dom->registerNodeClass("DOMElement", "Test");
} catch (ValueError $e) {
    echo "ValueError: ", $e->getMessage(), "\n";
}

$dom->createElement("foo");

?>
--EXPECT--
ValueError: DOMDocument::registerNodeClass(): Argument #2 ($extendedClass) must not be an abstract class
