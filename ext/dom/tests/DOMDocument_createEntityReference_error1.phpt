--TEST--
DomDocument::createEntityReference() - DOM_INVALID_CHARACTER_ERR raised if name contains an invalid character
--EXTENSIONS--
dom
--FILE--
<?php
$objDoc = new DOMDocument();

try {
    $objDoc->createEntityReference('!');
} catch (DOMException $e) {
    var_dump($e->getCode() === DOM_INVALID_CHARACTER_ERR);
    echo $e->getMessage();
}
?>
--EXPECT--
bool(true)
Invalid Character Error
