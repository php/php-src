--TEST--
Calling bzerr* functions on non-bz2 streams
--EXTENSIONS--
bz2
--FILE--
<?php
$f = fopen(__FILE__, 'r');
try {
    var_dump(bzerrno($f));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(bzerrstr($f));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    var_dump(bzerror($f));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
?>
--EXPECT--
TypeError: bzerrno(): Argument #1 ($bz) must be a bz2 stream
TypeError: bzerrstr(): Argument #1 ($bz) must be a bz2 stream
TypeError: bzerror(): Argument #1 ($bz) must be a bz2 stream
