--TEST--
Collator double construction should not be allowed
--EXTENSIONS--
intl
--FILE--
<?php
$collator = new Collator('en_US');

try {
    $collator->__construct('en_US');
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Collator object is already constructed
