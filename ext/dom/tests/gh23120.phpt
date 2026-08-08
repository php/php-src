--TEST--
GH-23120 (Stack overflow when comparing deeply nested DOM nodes)
--EXTENSIONS--
dom
--FILE--
<?php
function create_deep_document(): DOMDocument {
    $doc = new DOMDocument();
    $node = $doc->createElement('leaf', 'x');

    for ($i = 0; $i < 100000; $i++) {
        $parent = $doc->createElement('a');
        $parent->appendChild($node);
        $node = $parent;
    }

    $doc->appendChild($node);
    return $doc;
}

$doc1 = create_deep_document();
$doc2 = create_deep_document();

var_dump($doc1->documentElement->isEqualNode($doc2->documentElement));
?>
--EXPECT--
bool(true)
