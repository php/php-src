--TEST--
mkdir(): invalid mode
--FILE--
<?php
try {
    mkdir(__DIR__ . '/testdir', 1000000);
} catch (ValueError $e) {
    echo $e->getMessage(), PHP_EOL;
}
?>
--EXPECT--
mkdir(): Argument #2 ($permissions) must be between 0 and 0o7777
