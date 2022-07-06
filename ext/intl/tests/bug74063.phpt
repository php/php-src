--TEST--
Bug #74063 (NumberFormatter fails after retrieval from session)
--SKIPIF--
<?php
if (!extension_loaded('intl')) die('skip intl extension not available');
?>
--FILE--
<?php
$formatter = new NumberFormatter("en_GB", NumberFormatter::CURRENCY);
try {
    serialize($formatter);
} catch (Exception $ex) {
    echo $ex->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Serialization of 'NumberFormatter' is not allowed
