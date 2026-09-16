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
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Collator object is already constructed
