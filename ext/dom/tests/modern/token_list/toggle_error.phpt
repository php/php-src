--TEST--
TokenList: toggle errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$element = $dom->documentElement;
$list = $element->classList;

try {
    $list->toggle("\0");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $list->toggle("a b");
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Dom\TokenList::toggle(): Argument #1 ($token) must not contain any null bytes
DOMException: The token must not contain any ASCII whitespace
