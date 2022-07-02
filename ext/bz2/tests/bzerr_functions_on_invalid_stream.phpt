--TEST--
Calling bzerr* functions on non-bz2 streams
--SKIPIF--
<?php if (!extension_loaded("bz2")) print "skip"; ?>
--FILE--
<?php
$f = fopen(__FILE__, 'r');
try {
    var_dump(bzerrno($f));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(bzerrstr($f));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(bzerror($f));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
bzerrno(): Argument #1 ($bz) must be a bz2 stream
bzerrstr(): Argument #1 ($bz) must be a bz2 stream
bzerror(): Argument #1 ($bz) must be a bz2 stream
