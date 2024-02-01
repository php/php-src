--TEST--
DOM\XMLDocument::createFromFile 03
--EXTENSIONS--
dom
--FILE--
<?php

try {
    DOM\XMLDocument::createFromFile("%00");
} catch (ValueError $e) {
    echo $e->getMessage();
}

?>
--EXPECT--
DOM\XMLDocument::createFromFile(): Argument #1 ($path) must not contain percent-encoded NUL bytes
