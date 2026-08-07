--TEST--
Document::createElement() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = Dom\HTMLDocument::createEmpty();
try {
    $dom->createElement('');
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $dom->createElement('$');
} catch (DOMException $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
DOMException: Invalid Character Error
DOMException: Invalid Character Error
