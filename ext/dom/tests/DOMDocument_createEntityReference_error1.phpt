--TEST--
DomDocument::createEntityReference() - DOM_INVALID_CHARACTER_ERR raised if name contains an invalid character
--EXTENSIONS--
dom
--FILE--
<?php
$objDoc = new DomDocument();

try {
    $objDoc->createEntityReference('!');
} catch (Throwable $e) {
    var_dump($e->getCode() === DOM_INVALID_CHARACTER_ERR);
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
bool(true)
DOMException: Invalid Character Error
