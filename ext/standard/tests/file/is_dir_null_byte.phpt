--TEST--
is_dir() with filenames with null bytes
--FILE--
<?php

try {
    var_dump(is_dir("file_with_null_byte.tmp\0"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: is_dir(): Argument #1 ($filename) must not contain any null bytes
