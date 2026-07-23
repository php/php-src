--TEST--
Bug #64931 (phar_add_file is too restrictive on filename)
--EXTENSIONS--
phar
--INI--
phar.readonly=0
--FILE--
<?php

echo "Test\n";

@unlink(__DIR__."/bug64931.phar");
$phar = new Phar(__DIR__."/bug64931.phar");
$phar->addFile(__DIR__."/src/.pharignore", ".pharignore");
try {
    $phar->addFile(__DIR__."/src/.pharignore", ".phar/gotcha");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $phar->addFromString(".phar", "gotcha");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $phar->addFromString(".phar//", "gotcha");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $phar->addFromString(".phar\\", "gotcha");
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $phar->addFromString(".phar\0", "gotcha");
} catch (ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--CLEAN--
<?php
@unlink(__DIR__."/bug64931.phar");
?>
--EXPECT--
Test
BadMethodCallException: Cannot create any files in magic ".phar" directory
BadMethodCallException: Cannot create any files in magic ".phar" directory
BadMethodCallException: Cannot create any files in magic ".phar" directory
BadMethodCallException: Cannot create any files in magic ".phar" directory
ValueError: Phar::addFromString(): Argument #1 ($localName) must not contain any null bytes
