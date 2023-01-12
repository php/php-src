--TEST--
Bug #74063 (NumberFormatter fails after retrieval from session)
--EXTENSIONS--
intl
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
Serialization of 'NumberFormatter' is not allowed, unless you extend the class and provide a serialisation method
