--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - hierarchy variation
--EXTENSIONS--
dom
--FILE--
<?php
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
    $testElement->prepend($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->append($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->before($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->after($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $testElement->replaceWith($alone, $container);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $doc->saveXML();
?>
--EXPECT--
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
Hierarchy Request Error
<?xml version="1.0"?>
<container>
    <alone/>
    <child><testelement/></child>
</container>
