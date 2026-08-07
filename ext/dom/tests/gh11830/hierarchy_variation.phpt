--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - hierarchy variation
--EXTENSIONS--
dom
--FILE--
<?php
function test($method) {
    $doc = new DOMDocument;
    $doc->loadXML(<<<XML
    <?xml version="1.0"?>
    <container>
        <alone/>
        <child><testelement/></child>
    </container>
    XML);

    $container = $doc->documentElement;
    $alone = $container->firstElementChild;
    $testElement = $alone->nextElementSibling->firstElementChild;

    try {
        $testElement->$method($alone, $container);
    } catch (\DOMException $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }

    echo $doc->saveXML();
}

test("prepend");
test("append");
test("before");
test("after");
test("replaceWith");
?>
--EXPECT--
DOMException: Hierarchy Request Error
<?xml version="1.0"?>
DOMException: Hierarchy Request Error
<?xml version="1.0"?>
DOMException: Hierarchy Request Error
<?xml version="1.0"?>
DOMException: Hierarchy Request Error
<?xml version="1.0"?>
DOMException: Hierarchy Request Error
<?xml version="1.0"?>
