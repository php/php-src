--TEST--
Spoofchecker double construction should not be allowed
--EXTENSIONS--
intl
--SKIPIF--
<?php if (!class_exists("Spoofchecker")) print "skip"; ?>
--FILE--
<?php
$checker = new Spoofchecker();

try {
    $checker->__construct();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
Error: Spoofchecker object is already constructed
