--TEST--
Dom\XMLDocument::createFromFile 03
--EXTENSIONS--
dom
--FILE--
<?php

try {
    Dom\XMLDocument::createFromFile("%00");
} catch (ValueError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
Dom\XMLDocument::createFromFile(): Argument #1 ($path) must not contain percent-encoded NUL bytes
