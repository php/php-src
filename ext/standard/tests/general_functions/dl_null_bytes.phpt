--TEST--
dl() rejects null bytes in extension filename
--FILE--
<?php

try {
    dl("foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: dl(): Argument #1 ($extension_filename) must not contain any null bytes
