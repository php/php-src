--TEST--
GH-11830 (ParentNode methods should perform their checks upfront) - document variation
--EXTENSIONS--
dom
--FILE--
<?php
$otherDoc = new DOMDocument;
$otherDoc->loadXML(<<<XML
<?xml version="1.0"?>
<other/>
XML);

$otherElement = $otherDoc->documentElement;

$doc = new DOMDocument;
$doc->loadXML(<<<XML
<?xml version="1.0"?>
<container>
    <alone/>
    <child><testelement/></child>
</container>
XML);

$testElement = $doc->documentElement->firstElementChild->nextElementSibling->firstElementChild;

try {
    $doc->documentElement->firstElementChild->prepend($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->append($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->before($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->after($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

try {
    $doc->documentElement->firstElementChild->replaceWith($testElement, $otherElement);
} catch (\DOMException $e) {
    echo $e->getMessage(), "\n";
}

echo $otherDoc->saveXML();
echo $doc->saveXML();
?>
--EXPECT--
Wrong Document Error
Wrong Document Error
Wrong Document Error
Wrong Document Error
Wrong Document Error
<?xml version="1.0"?>
<other/>
<?xml version="1.0"?>
<container>
    <alone/>
    <child><testelement/></child>
</container>
