--TEST--
Element::insertAdjacentText() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
$foo = $dom->appendChild($dom->createElement("foo"));
try {
    $foo->insertAdjacentText("beforebegin", "bar");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

$foo->insertAdjacentText("afterbegin", "bar");
$foo->insertAdjacentText("beforeend", "baz");

echo $dom->saveHTML(), "\n";

var_dump($foo->firstChild->textContent);
var_dump($foo->firstChild->nextSibling->textContent);

?>
--EXPECT--
Cannot insert text as a child of a document
<foo>barbaz</foo>
string(3) "bar"
string(3) "baz"
