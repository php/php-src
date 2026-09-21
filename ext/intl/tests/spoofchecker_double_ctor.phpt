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
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Spoofchecker object is already constructed
