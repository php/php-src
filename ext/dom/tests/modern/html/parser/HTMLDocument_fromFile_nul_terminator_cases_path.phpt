--TEST--
Dom\HTMLDocument::createFromFile() - NUL terminator cases path
--EXTENSIONS--
dom
--FILE--
<?php

try {
    Dom\HTMLDocument::createFromFile("\0");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    Dom\HTMLDocument::createFromFile('%00');
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: Dom\HTMLDocument::createFromFile(): Argument #1 ($path) must not contain any null bytes
ValueError: Dom\HTMLDocument::createFromFile(): Argument #1 ($path) must not contain percent-encoded NUL bytes
