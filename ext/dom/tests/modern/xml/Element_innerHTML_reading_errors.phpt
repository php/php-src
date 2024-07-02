--TEST--
Test reading Element::$innerHTML on XML documents - error cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createEmpty();

function createContainer() {
    global $dom;
    $element = $dom->createElement("container");
    return $element;
}

function test($container) {
    try {
        var_dump($container->innerHTML);
    } catch (DOMException $e) {
        echo $e->getMessage(), "\n";
    }
}

$container = createContainer();
$container->append("Hello, \x01 world!");
test($container);

$container = createContainer();
$container->append($dom->createComment('Hello -- world'));
test($container);

$container = createContainer();
$container->append($dom->createComment('Hello world-'));
test($container);

$container = createContainer();
$container->append($dom->createComment('Hello world-'));
test($container);

$container = createContainer();
$container->append($dom->createComment("\x01"));
test($container);

$container = createContainer();
$legacy = new DOMDocument;
$container->append($dom->importLegacyNode($legacy->createProcessingInstruction('foo:bar', '?>')));
test($container);

$container = createContainer();
$legacy = new DOMDocument;
$container->append($dom->importLegacyNode($legacy->createProcessingInstruction('foo', '?>')));
test($container);

$container = createContainer();
$legacy = new DOMDocument;
$container->append($dom->importLegacyNode($legacy->createProcessingInstruction('xml', '')));
test($container);

$container = createContainer();
$legacy = new DOMDocument;
$container->append($dom->importLegacyNode($legacy->createProcessingInstruction('foo', "\x01")));
test($container);

$container = createContainer();
$container->append($dom->createElement("with:colon"));
test($container);

$container = createContainer();
$container->append($dom->createElementNS("http://www.w3.org/2000/xmlns/", "xmlns:colon"));
test($container);

$container = createContainer();
$element = $container->appendChild(createContainer());
$element->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:x", "http://www.w3.org/2000/xmlns/");
test($container);

$container = createContainer();
$element = $container->appendChild(createContainer());
$element->setAttributeNS("http://www.w3.org/2000/xmlns/", "xmlns:x", "");
test($container);

$container = createContainer();
$element = $container->appendChild(createContainer());
$element->setAttribute("with:colon", "value");
test($container);

$container = createContainer();
$element = $container->appendChild(createContainer());
$element->setAttribute("xmlns", "value");
test($container);

?>
--EXPECT--
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
The resulting XML serialization is not well-formed
