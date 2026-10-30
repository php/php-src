--TEST--
Canonicalize unattached node should fail
--EXTENSIONS--
dom
--FILE--
<?php

$d = \Dom\XMLDocument::createFromString('<root><child/></root>');
$child = $d->documentElement->firstChild;
$child->remove();

try {
    $child->C14N();
} catch (Dom\DOMException $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Canonicalization can only happen on nodes attached to a document.
