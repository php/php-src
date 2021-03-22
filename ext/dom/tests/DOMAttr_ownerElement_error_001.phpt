--TEST--
Read $ownerElement with null parent.
--CREDITS--
Travis Pew
# TestFest Atlanta 2009-05-14
--EXTENSIONS--
dom
--FILE--
<?php

$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);
$attr = $root->setAttribute('category', 'books');
$document->removeChild($root);
$root = null;
try {
    var_dump($attr->ownerElement);
} catch (\Error $e) {
    echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
Error: Couldn't fetch DOMAttr. Node no longer exists
