--TEST--
DOM\HTMLDocument::createFromFile() - NUL terminator cases path
--EXTENSIONS--
dom
--FILE--
<?php

try {
    DOM\HTMLDocument::createFromFile("\0");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    DOM\HTMLDocument::createFromFile('%00');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
DOM\HTMLDocument::createFromFile(): Argument #1 ($path) must not contain any null bytes
DOM\HTMLDocument::createFromFile(): Argument #1 ($path) must not contain percent-encoded NUL bytes
