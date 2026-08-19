--TEST--
TokenList: foreach by ref
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\XMLDocument::createFromString('<root class="A B C"/>');

try {
    foreach ($dom->documentElement->classList as &$class) {
    }
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: An iterator cannot be used with foreach by reference
