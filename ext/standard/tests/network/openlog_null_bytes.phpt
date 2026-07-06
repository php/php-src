--TEST--
openlog() rejects null bytes in prefix
--SKIPIF--
<?php
if (!function_exists("openlog")) die("skip openlog() is not available");
?>
--FILE--
<?php
try {
    openlog("foo\0bar", LOG_NDELAY, LOG_USER);
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
openlog(): Argument #1 ($prefix) must not contain any null bytes
