--TEST--
bzopen(): throw TypeError if filename contains null bytes
--EXTENSIONS--
bz2
--FILE--
<?php

try {
    bzopen("file\0", "w");
} catch (TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    bzopen("file\0", "r");
} catch (TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
bzopen(): Argument #1 ($file) must not contain null bytes
bzopen(): Argument #1 ($file) must not contain null bytes
