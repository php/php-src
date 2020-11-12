--TEST--
Cloning and serializing finfo is not supported
--FILE--
<?php

$finfo = new finfo;
var_dump($finfo->buffer("Test string"));
try {
    $finfo2 = clone $finfo;
    var_dump($finfo2->buffer("Test string"));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
try {
    $finfo3 = unserialize(serialize($finfo));
    var_dump($finfo3->buffer("Test string"));
} catch (Exception $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECTF--
string(%d) "%s"
Trying to clone an uncloneable object of class finfo
Serialization of 'finfo' is not allowed
