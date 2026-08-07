--TEST--
fileperms() with filenames with null bytes
--FILE--
<?php

try {
    var_dump(fileperms("file_with_null_byte.tmp\0"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: fileperms(): Argument #1 ($filename) must not contain any null bytes
