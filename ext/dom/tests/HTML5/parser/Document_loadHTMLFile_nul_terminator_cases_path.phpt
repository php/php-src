--TEST--
DOM\HTML5Document::loadHTMLFile() - NUL terminator cases path
--EXTENSIONS--
dom
--FILE--
<?php

$dom = new DOM\HTML5Document();

try {
    $dom->loadHTMLFile("\0");
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
$dom->loadHTMLFile('%00');

?>
--EXPECTF--
DOM\HTML5Document::loadHTMLFile(): Argument #1 ($filename) must not contain any null bytes

Warning: DOM\HTML5Document::loadHTMLFile(): URI must not contain percent-encoded NUL bytes in %s on line %d
