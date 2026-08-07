--TEST--
TokenList: contains errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$element = $dom->documentElement;
$list = $element->classList;

try {
    $list->contains("\0");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Dom\TokenList::contains(): Argument #1 ($token) must not contain any null bytes
