--TEST--
DomDocument::createEntityReference() - DOM_INVALID_CHARACTER_ERR raised if name contains an invalid character
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
$objDoc = new DomDocument();

try {
    $objDoc->createEntityReference('!');
} catch (DOMException $e) {
    echo $e->getMessage();
}
?>
--EXPECT--
Invalid Character Error
