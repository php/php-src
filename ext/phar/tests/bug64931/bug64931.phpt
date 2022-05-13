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
    echo "CAUGHT: ". $e->getMessage() ."\n";
}

try {
    $phar->addFromString(".phar", "gotcha");
} catch (Exception $e) {
    echo "CAUGHT: ". $e->getMessage() ."\n";
}

try {
    $phar->addFromString(".phar//", "gotcha");
} catch (Exception $e) {
    echo "CAUGHT: ". $e->getMessage() ."\n";
}

try {
    $phar->addFromString(".phar\\", "gotcha");
} catch (Exception $e) {
    echo "CAUGHT: ". $e->getMessage() ."\n";
}

try {
    $phar->addFromString(".phar\0", "gotcha");
} catch (ValueError $e) {
    echo "CAUGHT: ". $e->getMessage() ."\n";
}

?>
--CLEAN--
<?php
@unlink(__DIR__."/bug64931.phar");
?>
--EXPECT--
Test
CAUGHT: Cannot create any files in magic ".phar" directory
CAUGHT: Cannot create any files in magic ".phar" directory
CAUGHT: Cannot create any files in magic ".phar" directory
CAUGHT: Cannot create any files in magic ".phar" directory
CAUGHT: Phar::addFromString(): Argument #1 ($localName) must not contain any null bytes
