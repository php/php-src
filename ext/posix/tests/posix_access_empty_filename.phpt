--TEST--
posix_access(): empty filename throws a ValueError
--EXTENSIONS--
posix
--FILE--
<?php
try {
    posix_access("");
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
posix_access(): Argument #1 ($filename) must not be empty
