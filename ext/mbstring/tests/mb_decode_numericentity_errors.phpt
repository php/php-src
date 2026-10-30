--TEST--
mb_decode_numericentity() map errors
--EXTENSIONS--
mbstring
--FILE--
<?php
try {
    $convmap = array(0xFF, 0x2FFFF, 0); // 3 elements
    echo mb_decode_numericentity('str', $convmap, "UTF-8") . "\n";
} catch (ValueError $ex) {
    echo $ex->getMessage(), "\n";
}
try {
    $convmap = array(0xFF, "not an int", 0, 0); // 3 elements
    echo mb_decode_numericentity('str', $convmap, "UTF-8") . "\n";
} catch (ValueError $ex) {
    echo $ex->getMessage(), "\n";
}

?>
--EXPECT--
mb_decode_numericentity(): Argument #2 ($map) must have a multiple of 4 elements
mb_decode_numericentity(): Argument #2 ($map) must only be composed of values of type int
