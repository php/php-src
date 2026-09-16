--TEST--
Dom\XMLDocument::createFromFile 03
--EXTENSIONS--
dom
--FILE--
<?php

try {
    Dom\XMLDocument::createFromFile("%00");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Dom\XMLDocument::createFromFile(): Argument #1 ($path) must not contain percent-encoded NUL bytes
