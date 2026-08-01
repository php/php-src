--TEST--
Cloning and serializing finfo is not supported
--EXTENSIONS--
fileinfo
--FILE--
<?php

$finfo = new finfo;
var_dump($finfo->buffer("Test string"));
try {
    $finfo2 = clone $finfo;
    var_dump($finfo2->buffer("Test string"));
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
try {
    $finfo3 = unserialize(serialize($finfo));
    var_dump($finfo3->buffer("Test string"));
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECTF--
string(%d) "%s"
Error: Trying to clone an uncloneable object of class finfo
Exception: Serialization of 'finfo' is not allowed
