--TEST--
Read $ownerElement with null parent.
--CREDITS--
Travis Pew
# TestFest Atlanta 2009-05-14
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

$document = new DOMDocument;
$root = $document->createElement('root');
$document->appendChild($root);
$attr = $root->setAttribute('category', 'books');
$document->removeChild($root);
$root = null;
var_dump($attr->ownerElement);
?>
--EXPECTF--
Warning: Couldn't fetch DOMAttr. Node no longer exists in %s on line %d

Notice: Undefined property: DOMAttr::$ownerElement in %s on line %d
NULL
