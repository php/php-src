--TEST--
Bug #79503 (Memory leak on duplicate metadata)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new Phar(__DIR__ . '/bug79503.phar');
} catch (UnexpectedValueException $ex) {
    echo $ex::class, ': ', $ex->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
UnexpectedValueException: phar error: tar-based phar "%s%ebug79503.phar" has invalid metadata in magic file ".phar/.metadata.bin"
