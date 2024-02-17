--TEST--
Document::createElement() edge cases
--EXTENSIONS--
dom
--FILE--
<?php

$dom = DOM\HTMLDocument::createEmpty();
try {
    $dom->createElement('');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}
try {
    $dom->createElement('$');
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Invalid Character Error
Invalid Character Error
