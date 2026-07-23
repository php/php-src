--TEST--
IntlListFormatter double construction should not be allowed
--EXTENSIONS--
intl
--FILE--
<?php
$formatter = new IntlListFormatter('en_US');

try {
    $formatter->__construct('en_US');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: IntlListFormatter object is already constructed
