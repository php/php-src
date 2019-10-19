--TEST--
DOMEntityReference - DOM_INVALID_CHARACTER_ERR raised if name contains an invalid character
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
try {
    new DOMEntityReference('!');
} catch (DOMException $e) {
    var_dump($e->getCode() === DOM_INVALID_CHARACTER_ERR);
    echo $e->getMessage();
}
?>
--EXPECT--
bool(true)
Invalid Character Error
