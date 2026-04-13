--TEST--
Bug #62443 Crypt SHA256/512 Segfaults With Malformed Salt
--FILE--
<?php
try {
    crypt("foo", '$5$'.chr(0).'abc');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    crypt("foo", '$6$'.chr(0).'abc');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
crypt(): Argument #2 ($salt) must not contain any null bytes
crypt(): Argument #2 ($salt) must not contain any null bytes
