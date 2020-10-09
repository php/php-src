--TEST--
Bug #39863 (file_exists() silently truncates after a null byte)
--CREDITS--
Andrew van der Stock, vanderaj @ owasp.org
--FILE--
<?php

$filename = __FILE__ . chr(0). ".ridiculous";

try {
    var_dump(file_exists($filename));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
file_exists(): Argument #1 ($filename) must not contain any null bytes
