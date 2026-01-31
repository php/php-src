--TEST--
posix_mkfifo(): invalid mode argument
--SKIPIF--
<?php
if (!function_exists("posix_mkfifo")) {
    die("skip no posix_mkfifo()");
}
?>
--FILE--
<?php

// Negative mode
try {
    posix_mkfifo(__DIR__ . "/testfifo1", -1);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// Too large mode
try {
    posix_mkfifo(__DIR__ . "/testfifo2", 010000); // > 07777
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

// Garbage bits
try {
    posix_mkfifo(__DIR__ . "/testfifo3", 020000); // S_IFCHR bit
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
posix_mkfifo(): Argument #2 ($permissions) must be between 0 and 0o7777
posix_mkfifo(): Argument #2 ($permissions) must be between 0 and 0o7777
posix_mkfifo(): Argument #2 ($permissions) must be between 0 and 0o7777
