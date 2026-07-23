--TEST--
fileowner() with filenames with null bytes
--FILE--
<?php

try {
    var_dump(fileowner("file_with_null_byte.tmp\0"));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: fileowner(): Argument #1 ($filename) must not contain any null bytes
