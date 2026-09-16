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
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $list->replace("X", "\0");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

try {
    $list->replace("a b", "X");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Dom\TokenList::replace(): Argument #1 ($token) must not contain any null bytes
ValueError: Dom\TokenList::replace(): Argument #2 ($newToken) must not contain any null bytes
DOMException: The token must not contain any ASCII whitespace
