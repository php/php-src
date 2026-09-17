--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - attribute variation
--EXTENSIONS--
dom
--FILE--
<?php
$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container x="foo">
    <test/>
</container>
XML);

try {
    $doc->documentElement->firstElementChild->prepend($doc->documentElement->attributes[0]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->append($doc->documentElement->attributes[0]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->before($doc->documentElement->attributes[0]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->after($doc->documentElement->attributes[0]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->replaceWith($doc->documentElement->attributes[0]);
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

echo $doc->saveXML();
?>
--EXPECT--
DOMException: Hierarchy Request Error
DOMException: Hierarchy Request Error
DOMException: Hierarchy Request Error
DOMException: Hierarchy Request Error
DOMException: Hierarchy Request Error
<?xml version="1.0"?>
<container x="foo">
    <test/>
</container>
