--TEST--
Document::importNode() with unsupported node type
--EXTENSIONS--
dom
--FILE--
<?php

// For legacy documents this should warn
$dom = new DOMDocument();
var_dump($dom->importNode($dom));

// For modern documents this should throw
$dom = DOM\HTMLDocument::createEmpty();
try {
    $dom->importNode($dom);
} catch (DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
Warning: DOMDocument::importNode(): Cannot import: Node Type Not Supported in %s on line %d
bool(false)
Not Supported Error
