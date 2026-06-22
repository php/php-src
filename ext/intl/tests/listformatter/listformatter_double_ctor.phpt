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
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
IntlListFormatter object is already constructed
