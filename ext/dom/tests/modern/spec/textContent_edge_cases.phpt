--TEST--
textContent edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<container>text<?pi value?></container>');

echo "document text content: ";
var_dump($dom->textContent);
try {
    $dom->textContent = "foo";
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

$container = $dom->documentElement;

$text = $container->firstChild;
$pi = $text->nextSibling;

echo "text node text content: ";
var_dump($text->textContent);
echo "pi node text content: ";
var_dump($pi->textContent);

$text->textContent = NULL;
$pi->textContent = NULL;

echo "text node text content: ";
var_dump($text->textContent);
echo "pi node text content: ";
var_dump($pi->textContent);

$container->textContent = NULL;
echo $dom->saveXML(), "\n";

?>
--EXPECT--
document text content: NULL
Cannot modify readonly property DOM\XMLDocument::$textContent
text node text content: string(4) "text"
pi node text content: string(5) "value"
text node text content: string(0) ""
pi node text content: string(0) ""
<?xml version="1.0" encoding="UTF-8"?>
<container></container>
