--TEST--
pcntl_unshare() with wrong flag
--SKIPIF--
<?php
if (!extension_loaded("pcntl")) die("skip");
if (!extension_loaded("posix")) die("skip posix extension not available");
if (!function_exists("pcntl_unshare")) die("skip pcntl_unshare is not available");
?>
--FILE--
<?php

try {
    pcntl_unshare(42);
} catch (ValueError $exception) {
    echo $exception->getMessage() . "\n";
}

?>
--EXPECT--
pcntl_unshare(): Argument #1 ($flags) must be a combination of CLONE_* flags
