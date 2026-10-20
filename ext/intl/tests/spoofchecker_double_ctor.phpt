--TEST--
Spoofchecker double construction should not be allowed
--EXTENSIONS--
intl
--FILE--
<?php
$checker = new Spoofchecker();

try {
    $checker->__construct();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
Error: Spoofchecker object is already constructed
