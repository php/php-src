--TEST--
GH-24289 (finfo::file() reports the wrong argument number for an invalid $filename)
--EXTENSIONS--
fileinfo
--FILE--
<?php

$finfo = new finfo();
try {
    var_dump($finfo->file(''));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($finfo->file("\0"));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
finfo::file(): Argument #1 ($filename) must not be empty
finfo::file(): Argument #1 ($filename) must not contain any null bytes
