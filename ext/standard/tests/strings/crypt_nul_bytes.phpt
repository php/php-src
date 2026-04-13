--TEST--
crypt() rejects passwords and salts containing null bytes
--FILE--
<?php
try {
    crypt("foo\0bar", '$2y$05$CCCCCCCCCCCCCCCCCCCCC.');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    crypt("foo", '$2y$05$CCCCCCCCCCC' . "\0" . 'CCCCCCC.');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
crypt(): Argument #1 ($string) must not contain any null bytes
crypt(): Argument #2 ($salt) must not contain any null bytes
