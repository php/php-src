--TEST--
GH-24289 (finfo::file() reports the wrong argument number for an invalid $filename)
--EXTENSIONS--
fileinfo
--FILE--
<?php

$finfo = new finfo();
try {
    var_dump($finfo->file(''));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($finfo->file("\0"));
} catch (Throwable $e) {
    echo $e::class, ": ", $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
ValueError: finfo::file(): Argument #1 ($filename) must not be empty
ValueError: finfo::file(): Argument #1 ($filename) must not contain any null bytes
