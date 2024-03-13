--TEST--
TokenList: replace errors
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');
$element = $dom->documentElement;
$list = $element->classList;

try {
    $list->replace("\0", "X");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $list->replace("X", "\0");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    $list->replace("a b", "X");
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
DOM\TokenList::replace(): Argument #1 ($token) must not contain any null bytes
DOM\TokenList::replace(): Argument #2 ($newToken) must not contain any null bytes
The token must not contain any ASCII whitespace
