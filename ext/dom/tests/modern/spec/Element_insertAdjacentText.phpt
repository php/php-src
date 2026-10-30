--TEST--
Element::insertAdjacentText() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
$foo = $dom->appendChild($dom->createElement("foo"));
try {
    $foo->insertAdjacentText(Dom\AdjacentPosition::BeforeBegin, "bar");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

$foo->insertAdjacentText(Dom\AdjacentPosition::AfterBegin, "bar");
$foo->insertAdjacentText(Dom\AdjacentPosition::BeforeEnd, "baz");

echo $dom->saveHtml(), "\n";

var_dump($foo->firstChild->textContent);
var_dump($foo->firstChild->nextSibling->textContent);

?>
--EXPECT--
Cannot insert text as a child of a document
<foo>barbaz</foo>
string(3) "bar"
string(3) "baz"
