--TEST--
proc_open() rejects null bytes in cwd
--SKIPIF--
<?php
if (!function_exists("proc_open")) echo "skip proc_open() is not available";
?>
--FILE--
<?php

try {
    proc_open("does_not_matter", [], $pipes, "foo\0bar");
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
ValueError: proc_open(): Argument #4 ($cwd) must not contain any null bytes
