--TEST--
Test edge case offsets in DOMNamedNodeMap
--EXTENSIONS--
dom
--FILE--
<?php

$document = new DomDocument();
$root = $document->createElement('root');
$document->appendChild($root);
$root->setAttribute('attrib', 'value');
var_dump($root->attributes->length);
// Consistent with the method call
try {
    var_dump($root->attributes[-1]);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $root->attributes[][] = null;
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
int(1)
ValueError: must be between 0 and 2147483647
Error: Cannot access DOMNamedNodeMap without offset
