--TEST--
fileinode() with filenames with null bytes
--FILE--
<?php

try {
    var_dump(fileinode("file_with_null_byte.tmp\0"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: fileinode(): Argument #1 ($filename) must not contain any null bytes
