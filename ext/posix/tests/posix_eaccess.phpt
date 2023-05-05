--TEST--
posix_eaccess() with bogus paths
--EXTENSIONS--
posix
--SKIPIF--
<?php
if (!function_exists("posix_eaccess")) die("skip only platforms with posix_eaccess");
?>
--FILE--
<?php

try {
    posix_eaccess(str_repeat('bogus path', 1042));
} catch (ValueError $e) {
    echo $e->getMessage() . PHP_EOL;
}

?>
--EXPECT--
posix_eaccess(): Argument #1 ($filename) cannot be empty
