--TEST--
Dom\Document::adoptNode() rejects DTD declarations
--EXTENSIONS--
dom
--FILE--
<?php
$xml = '<!DOCTYPE root [<!ELEMENT root EMPTY><!ENTITY ent "value">]><root/>';
$source = Dom\XMLDocument::createFromString($xml);
$target = Dom\XMLDocument::createFromString('<target/>');

$nodes = [
    $source->doctype->childNodes->item(0),
    $source->doctype->entities->item(0),
];

foreach ($nodes as $node) {
    try {
        $target->adoptNode($node);
        echo "adopted\n";
    } catch (DOMException $e) {
        echo $e::class, ": ", $e->getCode(), "\n";
    }
}
?>
--EXPECT--
DOMException: 9
DOMException: 9
